#include "canhw/canhw.h"
#include "stdio.h"
#include "string.h"
#include "can.h"
#include "utils/ve_logger.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
The LinCAN is an implementation of the Linux device driver supporting more CAN controller chips and many CAN interface boards. The driver roots can be traced to LDDK project. The OCERA version of the LinCAN driver adds new features, continuous enhancements and reimplementation of structure of the driver. The usage of the driver is tightly coupled to the virtual CAN API interface component which hides driver low level interface to the application programmers. Driver enables multiple opens of each communication objects from more Linux and RT-Linux applications. The message processing is based on the oriented graph of FIFOs concept.

http://sourceforge.net/project/showfiles.php?group_id=51629&package_id=108731
*/

static int fd = ZERO;

void veLcInit(void)
{
	fd = ZERO;
}

veBool veLcOpen(nc8 const * const can_dev_name)
{
	if ((fd=open(can_dev_name, O_RDWR)) < 0)
	{
		logE("LinCAN", "could not open %s", can_dev_name );
		return veFalse;
	}

	return veTrue;
}

// Set bus-parameters, mainly speed
veBool veLcSetBusParams(
				sn8 freq,
				un8 tseg1,
				un8 tseg2,
				un8 sjw,
				un8 noSamp,
				un8 syncmode
			)
{
	return veTrue;
}

// Go for it!
veBool veLcBusOn(void)
{
	//return canBusOn(activeHandle) == canOK;
	return veTrue;
}

// Go for it!
veBool veLcBusOff(void)
{
	//return canBusOn(activeHandle) == canOK;
	return veTrue;
}

void veLcDeInit(void)
{
	if (fd != ZERO)
		close(fd);
}

// CAN send normal message
veBool veLcCanSend(VeRawCanMsg* rawCanMsg)
{
	struct canmsg_t sendmsg;

	// set flags
	sendmsg.flags = 0;
	if (rawCanMsg->flags & VE_CAN_MSG_EXT)
		sendmsg.flags |= MSG_EXT;

	if (rawCanMsg->flags & VE_CAN_MSG_RTR)
		sendmsg.flags |= MSG_RTR;

	if ( rawCanMsg->dlc > 8)
	{
		logE("CANHW-lincan:", "DLC cannot be larger then 8!");
		return veFalse;
	}

	// copy rest of data
	sendmsg.id = rawCanMsg->canId;
	sendmsg.length = rawCanMsg->dlc;
	memcpy(sendmsg.data, rawCanMsg->mdata, sendmsg.length);

	if ( write(fd, &sendmsg, sizeof(struct canmsg_t)) < 0)
		return veFalse;

	return veTrue;
}

// check if a new message is available
veBool veLcCanRead(VeRawCanMsg * rawCanMsg)
{
	struct canmsg_t readmsg={0,0,5,{0,0},0,{0,}};
	int ret;

	// poll if ready
	struct timeval timeout;
	fd_set set;

	FD_ZERO (&set);
	FD_SET (fd, &set);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	// Check if the file is ready (prevent blocking call to read)
	// continue when -1, and errno = EINTR since this is a valid exception
	while ((ret=select(FD_SETSIZE,&set, NULL, NULL, &timeout)) ==-1  && errno == EINTR);

	// do not block!
	if (ret <= 0)
		return veFalse;

	readmsg.flags = 0;
	readmsg.cob = 0;
	ret = read(fd, &readmsg, sizeof(struct canmsg_t));

	if (ret <= 0)           // error or no msg
		return veFalse;

	// Type of message
	rawCanMsg->dlc = readmsg.length;
	rawCanMsg->flags = 0;
	if (readmsg.flags & MSG_EXT)
		rawCanMsg->flags |= VE_CAN_MSG_EXT;
	else
		rawCanMsg->flags |= VE_CAN_MSG_STD;

	// Remote request
	if (readmsg.flags & MSG_RTR)
		rawCanMsg->flags |= VE_CAN_MSG_RTR;

	// TODO: error and overflow

	memcpy(rawCanMsg->mdata, readmsg.data, readmsg.length);
	rawCanMsg->canId = readmsg.id;

	return veTrue;
}

// functions to select the active bus
void veLcSetHandle(int newfd)
{
	fd = newfd;
}

int veLcGetHandle (void)
{
	return fd;
}
