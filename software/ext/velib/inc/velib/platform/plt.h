#ifndef _VELIB_PLATFORM_PLT_H_
#define _VELIB_PLATFORM_PLT_H_

#include <velib/base/types.h>
#include <velib/velib_config.h>

/* Common for embedded and operation systems */
void pltInit(void);
veBool pltIsTick50ms(void);
un16 pltGetCount1ms(void);

/**
 * Just to provide a common prototype, needed for a
 * bootloader to exit properly, not commenly used. Either
 * use pltExit for pc tools or keep running on embedded boards
 * till a brown out.y
 */
void pltDeInit(void);

#ifndef CFG_NO_PREEMPTION

/* common, intended to be called once after init! */
void pltInterruptEnable(void);

/* For interrupt safety, should be paired */
void pltInterruptSuspend(void);
void pltInterruptResume(void);

/* Enter the low power IDLE mode, and resume on interrupt */
void pltIdleMode(void);

void pltInterruptAssertDisabled(int level);

#else

#define pltInterruptEnable()			do {} while(0)
#define pltInterruptSuspend()			do {} while(0)
#define pltInterruptResume()			do {} while(0)
#define pltIdleMode()					do {} while(0)
#define pltInterruptAssertDisabled(a)	do {} while(0)

#endif

/*
 * Initialises the mutex / semaphores to emulated interrupt behaviour.
 * Bare bone projects don't need this since the silicon takes care of it.
 */
VE_DCL void pltInitCtx(void);

#ifdef CFG_HAVE_INTERRUPTS
# define pltInterruptBegins(l)		do {} while(0)
# define pltInterruptEnds(l)		do {} while(0)
#else
/* Used by threads, to emulated interrupts. */
void pltInterruptBegins(un8 level);
void pltInterruptEnds(un8 level);
#endif

/* Provides a way to pass the serial device to the application */
VE_DCL char const *pltGetSerialDevice(void);
VE_DCL char const *pltProgramName(void);
char const *pltProgramVersion(void);

void pltWatchFileDescriptor(int fd);

/* Only useful when libevent is used */
struct event_base;
struct event_base *pltGetLibEventBase(void);

#endif
