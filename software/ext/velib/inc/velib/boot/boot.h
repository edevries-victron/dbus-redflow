#ifndef _VELIB_BOOT_BOOT_H_
#define _VELIB_BOOT_BOOT_H_

#include <velib/utils/ve_events.h>
#include <velib/J1939/J1939.h>

typedef enum
{
	BOOT_APP_VALIDATING,
	BOOT_UPD_WAIT,
	BOOT_DATA_WAIT,
	BOOT_DATA_HANDLE,
	BOOT_DATA_SEND_ACK
} BootState;

typedef struct
{
	BootState	state;		///< progress of the bootloader
	un16		timer;		///< counter for timeouts
	un16		ackCode;	///< (last) error codes during update
	J1939Nad	updNad;		///< Network address of the sending node
	veBool		updReq;
	un32		fwVersion;
	veBool		panic;		///< flag to omit additional checks
} BootVars;

extern BootVars veBoot;

void	bootInit(void);
#define bootTick()			bootHandler(VE_EV_TICK)
#define bootUpdate()		bootHandler(VE_EV_UPDATE)

void	bootVRegHandler(VeEvent ev);
void	bootHandler(VeEvent ev);

// callback -> ready to boot
void	bootEvent(void);
void	bootUpdEnterEvent(void);
void	bootUpdEndEvent(void);

#endif
