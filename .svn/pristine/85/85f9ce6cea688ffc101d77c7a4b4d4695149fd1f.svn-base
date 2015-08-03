#include <velib/base/ve_string.h>
#include <velib/platform/plt.h>
#include <velib/platform/serial.h>
#include <velib/types/ve_str.h>
#include <velib/utils/ve_logger.h>

#include "serial_internal.h"

/* see main_windows */
extern veBool pltAddWinHandle(HANDLE handle, void (*cb)(void *ctx), void *ctx);
extern veBool pltRemoveWinHandle(HANDLE handle);

static un32 sPutBuf(VeSerialPort* port, un8 const* buf, un32 len)
{
	DWORD n;
	OVERLAPPED ov, *pov = NULL;
	DWORD error;

	if (port->intLevel == 0) {
		pov = &ov;
		ov.hEvent = CreateEvent(0, TRUE, 0, 0);
	}

	if (!WriteFile(port->nt.win.hCom, buf, len, &n, pov)) {
		error = GetLastError();
		if (error == ERROR_IO_PENDING) {
			WaitForSingleObject(ov.hEvent, INFINITE);
		} else {
			n = 0;
		}
	}

	if (pov)
		CloseHandle(ov.hEvent);

	return (un32) n;
}

static veBool sPutByte(VeSerialPort* port, un8 byte)
{
	return sPutBuf(port, &byte, 1) == 1;
}

static veBool sSetBaud(VeSerialPort* port, un32 baudrate)
{
	DCB	dcb;
	veBool ret = veFalse;

	pltInterruptSuspend();

	logI("serial", "changing baud to %d", baudrate);
	if (GetCommState(port->nt.win.hCom, &dcb))
	{
		dcb.BaudRate = baudrate;
		ret = SetCommState(port->nt.win.hCom, &dcb) == TRUE;
	}

	pltInterruptResume();

	if (!ret)
		logE("serial", "baud rate could not be set");

	return ret;
}

static veBool sBreak(VeSerialPort* port)
{
	veBool ret;

	pltInterruptSuspend();
	ret = EscapeCommFunction(port->nt.win.hCom, SETBREAK) != 0;
	pltInterruptResume();

	if (!ret)
		logE("serial", "could not break");

	return ret;
}

static veBool sBreakClear(VeSerialPort* port)
{
	veBool ret;

	pltInterruptSuspend();
	ret = EscapeCommFunction(port->nt.win.hCom, CLRBREAK) != 0;
	PurgeComm(port->nt.win.hCom, PURGE_RXCLEAR | PURGE_RXABORT);
	pltInterruptResume();
	return ret;
}

static veBool sClose(VeSerialPort* port)
{
	if (port->nt.win.hCom == INVALID_HANDLE_VALUE)
		return veTrue;

	if (port->intLevel == 0) {
		pltRemoveWinHandle(port->nt.win.ov.hEvent);
		CloseHandle(port->nt.win.ov.hEvent);
	} else {
		port->nt.win.exit = veTrue;
		pltInterruptResume();
		while (port->nt.win.hThread != INVALID_HANDLE_VALUE);
		pltInterruptSuspend();
	}

	CloseHandle(port->nt.win.hCom);
	port->nt.win.hCom = INVALID_HANDLE_VALUE;

	return veFalse;
}

static  VeVariant *sDeviceNumber(struct VeSerialPortS* port, VeVariant *var)
{
	const char *p;

	var->type.tp = VE_UNKNOWN;

	p = strstr(port->dev, "COM");
	if (!p)
		p = strstr(port->dev, "com");

	if (p) {
		sn16 devNr;
		if (sscanf(p + 3, "%hd", &devNr) == 1)
			return veVariantUn16(var, 256 + devNr);
	}

	return var;
}

static UINT WINAPI serialThread(void* parm)
{
	VeSerialPort* port = (VeSerialPort*) parm;
	un8 buf[2048];

	while(!port->nt.win.exit)
	{
		DWORD n;
		DWORD dwCommEvent;

		WaitCommEvent(port->nt.win.hCom, &dwCommEvent, NULL);

		for (;;) {
			if (ReadFile(port->nt.win.hCom, &buf, sizeof(buf), &n, NULL))
			{
				if (n == 0)
					break;
				veSerialHandleData(port, buf, n);
			} else {
				VeStr s;

				veStrNewFormat(&s, "error: %d", GetLastError());
				veSerialHandleError(port, veStrCStr(&s));
				veStrFree(&s);
			}
		};
	}

	port->nt.win.hThread = INVALID_HANDLE_VALUE;
	return 0;
}

static veBool sSetEol(VeSerialPort* port, sn16 eol)
{
	DCB dcb;

	if (eol >= 0) {
		if (!GetCommState(port->nt.win.hCom, &dcb))
			return veFalse;

		dcb.EvtChar = (char) eol;

		if (!SetCommState(port->nt.win.hCom, &dcb))
			return veFalse;

		if (!SetCommMask(port->nt.win.hCom, EV_RXFLAG))
			return veFalse;
	} else {
		if (!SetCommMask(port->nt.win.hCom, EV_RXCHAR))
			return veFalse;
	}

	return veTrue;
}

static void onSerialEvent(VeSerialPort* port)
{
	DWORD n;
	un8 buf[2048];
	OVERLAPPED ov = {0};

	ov.hEvent = CreateEvent(0, TRUE, FALSE, 0);
	do {
		if (!ReadFile(port->nt.win.hCom, &buf, sizeof(buf), &n, &ov)) {
			DWORD error = GetLastError();
			if (error == ERROR_IO_PENDING)
				WaitForSingleObject(ov.hEvent, INFINITE);
		}
		if (n != 0)
			veSerialHandleData(port, buf, n);
	} while (n != 0);
	CloseHandle(ov.hEvent);

	ResetEvent(port->nt.win.ov.hEvent);
	WaitCommEvent(port->nt.win.hCom, &port->nt.win.eventMask, &port->nt.win.ov);
}

/* Open a COM port */
veBool veSerialWinOpen(VeSerialPort* port)
{
	DCB	dcb;
	COMMTIMEOUTS cto;
	WCHAR comport[40];

	MultiByteToWideChar(0,0, port->dev, -1, comport, sizeof(comport));

	port->nt.win.hCom = CreateFile(
			comport,
			GENERIC_READ | GENERIC_WRITE,
			0,											// exclusive access
			NULL,										// no security
			OPEN_EXISTING,
			port->intLevel == 0 ? FILE_FLAG_OVERLAPPED : 0,
			NULL
		);

	if (port->nt.win.hCom == INVALID_HANDLE_VALUE)
		return veFalse;

	// NOTE: Make sure the RX queue size is smaller than or equal to
	// the size of sBuffer, since otherwise you will loose data.
	if (!SetupComm(port->nt.win.hCom, 1024, 1024))
		return veFalse;

	PurgeComm(port->nt.win.hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR
				| PURGE_RXCLEAR );

	if (!GetCommState(port->nt.win.hCom, &dcb))
		return veFalse;

	dcb.BaudRate = port->baudrate;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = FALSE;
	dcb.XonLim = 0;
	dcb.XoffLim = 0;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	if (!SetCommState(port->nt.win.hCom, &dcb))
		return veFalse;

	// Determine the time-outs
	if (!GetCommTimeouts(port->nt.win.hCom, &cto))
		return veFalse;

	// Set the new timeouts
	cto.ReadIntervalTimeout = MAXDWORD;
	cto.ReadTotalTimeoutConstant = 0;
	cto.ReadTotalTimeoutMultiplier = 0;

	cto.WriteTotalTimeoutConstant = 0;
	cto.WriteTotalTimeoutMultiplier = 0;

	// Set the new DCB structure
	if (!SetCommTimeouts(port->nt.win.hCom, &cto))
		return veFalse;

	if (!SetCommMask(port->nt.win.hCom, EV_RXCHAR))
		return veFalse;

	port->close = sClose;
	port->putByte = sPutByte;
	port->putBuf = sPutBuf;
	port->setBaud = sSetBaud;
	port->setBreak = sBreak;
	port->clearBreak = sBreakClear;
	port->getDeviceNumber = sDeviceNumber;
	port->setEol = sSetEol;

	if (port->intLevel == 0) {
		port->nt.win.ov.hEvent = CreateEvent(0, TRUE, 0, 0);
		pltAddWinHandle(port->nt.win.ov.hEvent, (void (*)(void *)) onSerialEvent, port);
		WaitCommEvent(port->nt.win.hCom, &port->nt.win.eventMask, &port->nt.win.ov);
	} else {
		port->nt.win.hThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE) serialThread,
			port,
			0,
			&port->nt.win.threadId
		);
	}

	return veTrue;
}
