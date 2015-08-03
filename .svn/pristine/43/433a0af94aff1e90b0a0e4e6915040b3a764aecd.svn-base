#ifndef _VELIB_PLATFORM_SERIAL_WIN_H_
#define _VELIB_PLATFORM_SERIAL_WIN_H_

#ifndef _VELIB_PLATFORM_SERIAL_H_
#error include velib/platform/serial.h instead.
#endif

#ifdef _WIN32

typedef struct
{
	HANDLE hCom;
	HANDLE hThread;
	DWORD threadId;
	veBool exit;
	OVERLAPPED ov;
	DWORD eventMask;
} SerialPortWin;

veBool veSerialWinOpen(struct VeSerialPortS* port);
#else

typedef void* SerialPortWin;

#endif
#endif
