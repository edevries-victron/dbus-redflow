#ifndef _VELIB_PLATFORM_SERIAL_POSIX_H_
#define _VELIB_PLATFORM_SERIAL_POSIX_H_

#ifndef _VELIB_PLATFORM_SERIAL_H_
#error include velib/platform/serial.h instead.
#endif

#ifdef _UNIX_

#include <pthread.h>

typedef struct
{
	pthread_t thread;
} posixSerialPort;

veBool veSerialPosixOpen(struct VeSerialPortS* port);
#else

typedef int posixSerialPort;

#endif
#endif
