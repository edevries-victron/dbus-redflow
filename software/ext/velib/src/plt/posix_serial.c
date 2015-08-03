#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <velib/base/types.h>
#include <velib/base/ve_string.h>
#include <velib/platform/console.h>
#include <velib/platform/plt.h>
#include <velib/platform/serial.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_logger.h>

#include "serial_internal.h"

#if CFG_WITH_LIBEVENT
#include <event2/event.h>
#endif

#define MODULE "serial"

typedef struct
{
	speed_t speed;
	un32 value;
}  PosixBaudRate;

static PosixBaudRate const baudRates[] =
{
	{B0,			0},
	{B50,			50},
	{B75,			75},
	{B110,			110},
	{B134,			134},
	{B150,			150},
	{B200,			200},
	{B300,			300},
	{B600,			600},
	{B1200,			1200},
	{B1800,			1800},
	{B2400,			2400},
	{B4800,			4800},
	{B9600,			9600},
	{B19200,		19200},
	{B38400,		38400},
#ifdef B57600
	{B57600,		57600},
#endif
#ifdef B115200
	{B115200,		115200},
#endif
#ifdef B230400
	{B230400,		230400},
#endif
#ifdef B460800
	{B460800,		460800},
#endif
#ifdef B500000
	{B500000,		500000},
#endif
#ifdef B576000
	{B576000,		576000},
#endif
#ifdef B921600
	{B921600,		921600},
#endif
#ifdef B1000000
	{B1000000,		1000000},
#endif
#ifdef B1152000
	{B1152000,		1152000},
#endif
#ifdef B1500000
	{B1500000,		1500000},
#endif
#ifdef B2000000
	{B2000000,		2000000},
#endif
#ifdef B2500000
	{B2500000,		2500000},
#endif
#ifdef B3000000
	{B3000000,		3000000},
#endif
#ifdef B3500000
	{B3500000,		3500000},
#endif
#ifdef B4000000
	{B4000000,		4000000},
#endif
	{0, 0}
};


speed_t findSpeed(un32 baudrate)
{
	un8 n = 0;

	while(baudRates[++n].value)
		if (baudRates[n].value == baudrate)
			return baudRates[n].speed;
	return 0;
}

static veBool sClose(VeSerialPort* port)
{
#if CFG_WITH_LIBEVENT
	event_free((struct event *) port->event);
#endif
	return close(port->fh) >= 0;
}

static un32 sPutBuf(VeSerialPort* port, un8 const* buf, un32 len)
{
	ssize_t ret;
	do {
		ret = write(port->fh, buf, len);
		if (ret == -1 && errno != EINTR && errno != EAGAIN) {
			logE(MODULE, "serial write error: %d", errno);
			return veFalse;
		}
		len -= ret;
	} while (len);
	return veTrue;
}

static veBool sPutByte(VeSerialPort* port, un8 byte)
{
	ssize_t ret;
	do {
		ret = write(port->fh, &byte, sizeof(byte));
		if (ret == -1 && errno != EINTR && errno != EAGAIN) {
			logE(MODULE, "serial write error: %d", errno);
			return veFalse;
		}
	} while (ret != 1);
	return veTrue;
}

static veBool sSetBaud(VeSerialPort* port, un32 baudrate)
{
	struct termios tio;
	speed_t speed;

	speed = findSpeed(baudrate);
	if (!speed) {
		logE(MODULE, "baud rate supported");
		return veFalse;
	}

	tcgetattr(port->fh, &tio);
	port->baudrate = baudrate;
	cfsetispeed(&tio, speed);
	cfsetospeed(&tio, speed);
	tcsetattr(port->fh, TCSANOW, &tio);

	return veTrue;
}

static veBool sBreak(VeSerialPort* port)
{
	return ioctl(port->fh, TIOCSBRK) == 0;
}

static veBool sBreakClear(VeSerialPort* port)
{
	return ioctl(port->fh, TIOCCBRK) == 0;
}

static veBool sSetEol(VeSerialPort* port, sn16 eol)
{
	struct termios tio;

	if (tcgetattr(port->fh, &tio) != 0)
		return veFalse;

	if (eol >= 0)
		tio.c_cc[VEOL] = eol;
	else
		tio.c_cc[VEOL] = _POSIX_VDISABLE;

	tio.c_lflag = ICANON;

	return (tcsetattr(port->fh, TCSANOW, &tio) == 0);
}

static VeVariant *sDeviceNumber(VeSerialPort* port, VeVariant *var)
{
	sn32 devNr;

	var->type.tp = VE_UNKNOWN;

	if (ve_strnicmp(port->dev, "/dev/ttyO", 9) == 0) {
		if (sscanf(port->dev + 9, "%i", &devNr) == 1)
			return veVariantUn16(var, 256 + devNr);
		return var;
	}

	if (ve_strnicmp(port->dev, "/dev/ttyS", 9) == 0) {
		if (sscanf(port->dev + 9, "%i", &devNr) == 1)
			return veVariantUn16(var, 256 + 16 + devNr);
		return var;
	}

	if (ve_strnicmp(port->dev, "/dev/ttyUSB", 11) == 0) {
		if (sscanf(port->dev + 11, "%i", &devNr) == 1)
			return veVariantUn16(var, 256 + 32 + devNr);
		return var;
	}

	return var;
}

#if !defined(CFG_INIT_CTX) || CFG_INIT_CTX == 0

static veBool spawnThread(VeSerialPort* port)
{
	VE_UNUSED(port);

	logE(MODULE, "not compiled with thread support!");

	return veFalse;
}

#else

static void *serialThread(void *parm)
{
	VeSerialPort* port = (VeSerialPort*) parm;

	for (;;) {
		int retVal;
		fd_set readSet;
		fd_set errorSet;

		FD_ZERO(&readSet);
		FD_SET(port->fh, &readSet);

		FD_ZERO(&errorSet);
		FD_SET(port->fh, &errorSet);

		retVal = select(FD_SETSIZE, &readSet, NULL, &errorSet, NULL);
		if (retVal == -1) {
			if (errno == EINTR)
				continue;
			veSerialHandleError(port, "Error using select");
			goto out;
		}

		if (FD_ISSET(port->fh, &errorSet)) {
			veSerialHandleError(port, "Error on serial port");
			goto out;
		}

		if (FD_ISSET(port->fh, &readSet)) {
			un8 buf[2048];
			ssize_t len;

			len = read(port->fh, buf, sizeof(buf));
			if (!veSerialHandleData(port, buf, len))
				goto out;
		}
	}

out:
	close(port->fh);

	 /* FIXME, should go, but only after applications use the event callback */
	logE(MODULE, "serial error -> brutally exiting the whole application!");
	pltExit(4);

	return NULL;
}

static veBool spawnThread(VeSerialPort* port)
{
	pthread_attr_t attr;
	int rc;

	logI(MODULE, "starting thread");
	pthread_attr_init(&attr);
	rc = pthread_create(&port->nt.posix.thread, &attr, serialThread, port);

	return rc == 0;
}

#endif

#if CFG_WITH_LIBEVENT

static void rx_data(evutil_socket_t fd, short what, void *arg)
{
	ssize_t len;
	un8 buf[2048];
	VeSerialPort *port = (VeSerialPort *) arg;
	veAssert((what & EV_READ) != 0);
	VE_UNUSED(fd);
	VE_UNUSED(what);

	len = read(port->fh, buf, sizeof(buf));
	veSerialHandleData(port, buf, len);
}

static veBool registerLibEvent(VeSerialPort *port)
{
	struct event *rxEvent;
	struct event_base *base = pltGetLibEventBase();

	rxEvent = event_new(base, port->fh, EV_READ|EV_PERSIST, rx_data, port);
	if (rxEvent == NULL)
		return veFalse;
	port->event = (void *) rxEvent;
	return event_add(rxEvent, NULL) == 0;
}

#else

static veBool registerLibEvent(VeSerialPort *port)
{
	VE_UNUSED(port);

	logE(MODULE, "not compiled with libevent support!");

	return veFalse;
}

#endif

veBool veSerialPosixOpen(VeSerialPort* port)
{
	struct termios newtio; /* new port settings for serial port */
	speed_t speed;

	port->close = sClose;
	port->putByte = sPutByte;
	port->putBuf = sPutBuf;
	port->setBaud = sSetBaud;
	port->setBreak = sBreak;
	port->clearBreak = sBreakClear;
	port->getDeviceNumber = sDeviceNumber;
	port->setEol = sSetEol;

	if (strcmp(port->dev, "-") == 0)
		port->dev = ttyname(STDIN_FILENO);

	/*
	 * Open modem device for reading and writing and not as controlling tty
	 * because we don't want to get killed if linenoise sends CTRL-C.
	 */
	port->fh = open(port->dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (port->fh < 0)
	{
		logE(MODULE, "could not open com-port");
		return veFalse;
	}

	// unset FNDELAY (only needed for open not waiting for the modem to be ready...)
	fcntl(port->fh, F_SETFL, 0);

	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

	/*
	 * BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
	 * CRTSCTS : output hardware flow control (only used if the cable has
	 * all necessary lines. See sect. 7 of Serial-HOWTO)
	 * CS8     : 8n1 (8bit,no parity,1 stopbit)
	 * CLOCAL  : local connection, no modem contol
	 * CREAD   : enable receiving characters
	 */
	newtio.c_cflag = CS8 | CREAD | CLOCAL;

	if (port->baudrate) {
		speed = findSpeed(port->baudrate);
		if (!speed) {
			logE(MODULE, "baud rate supported");
			return veFalse;
		}

		cfsetispeed(&newtio, speed);
		cfsetospeed(&newtio, speed);
	}

	/*
	 * IGNPAR  : ignore bytes with parity errors
	 * ICRNL   : map CR to NL (otherwise a CR input on the other computer
	 * will not terminate input)
	 * otherwise make device raw (no other input processing)
	*/
	newtio.c_iflag = 0; //IGNPAR;

	/* Raw output. */
	newtio.c_oflag = 0;

	/*
	 * ICANON  : enable canonical input
	 * disable all echo functionality, and don't send signals to calling program
	 */
	newtio.c_lflag = 0; /* ICANON; */

	/*
	 * initialize all control characters
	 * default values can be found in /usr/include/termios.h, and are given
	 * in the comments, but we don't need them here
	 */

	/*
	 * newtio.c_cc[VINTR]    = _POSIX_VDISABLE;
	 * newtio.c_cc[VQUIT]    = _POSIX_VDISABLE;
	 * newtio.c_cc[VERASE]   = _POSIX_VDISABLE;
	 * newtio.c_cc[VKILL]    = _POSIX_VDISABLE;
	 * newtio.c_cc[VEOF]     = _POSIX_VDISABLE;
	 */

	/*
	 * VMIN > 0 and VTIME = 0
	 * A blocking read until at least VMIN characters are available for reading.
	 */
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;

	/*
	 * newtio.c_cc[VSWTC]    = _POSIX_VDISABLE;
	 * newtio.c_cc[VSTART]   = _POSIX_VDISABLE;
	 * newtio.c_cc[VSTOP]    = _POSIX_VDISABLE;
	 * newtio.c_cc[VSUSP]    = _POSIX_VDISABLE;
	 * newtio.c_cc[VEOL]     = 0xC1;
	 * newtio.c_cc[VREPRINT] = _POSIX_VDISABLE;
	 * newtio.c_cc[VDISCARD] = _POSIX_VDISABLE;
	 * newtio.c_cc[VWERASE]  = _POSIX_VDISABLE;
	 * newtio.c_cc[VLNEXT]   = _POSIX_VDISABLE;
	 * newtio.c_cc[VEOL2]    = _POSIX_VDISABLE;
	 */

	/* now clean the modem line and activate the settings for the port */
	tcflush(port->fh, TCIOFLUSH);
	tcsetattr(port->fh, TCSANOW, &newtio);

	if (port->intLevel > 0)
		return spawnThread(port);

	return registerLibEvent(port);
}
