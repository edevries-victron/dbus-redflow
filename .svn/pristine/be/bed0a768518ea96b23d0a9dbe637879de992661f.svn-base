#include <velib/platform/plt.h>
#include <velib/velib_config.h>
#include "platform.h"
#include "intrinsics_hc08.h"
#include <stdlib.h>

extern void MCU_init(void);

volatile veBool tick;
volatile un16 ms;
volatile un16 us200;

void pltInit(void)
{
	MCU_init();
	CANRIER_RXFIE = CFG_CANHW_RX_INTERRUPT;
	CANTIER_TXEIE0 = CFG_CANHW_TX_INTERRUPT;

	/* set counter to 200 us and generate an interrupt every 50ms */
	RTCMOD = (250 - 1);
	_RTCSC.MergedBits.grpRTCPS = 9; /* 2 * 10^3 */

}

un16 pltGetCount1ms(void)
{
	un16 ret;

	pltInterruptSuspend();
	ret = RTCCNT;
	if (RTCSC_RTIF)
			ret = 5 * 50;
	ret /= 5;
	ret += ms;
	pltInterruptResume();

	return ret;
}

un8 veRand(void)
{
	un16 ret;

	srand(RTCCNT);
	ret = rand();
	
	return (un8) ret;
}

static volatile un8 interruptCount;
static un8 interruptState;

void pltInterruptEnable(void)
{
	asm CLI;
}

/*
 * The hc08 clears it interrupt pending flag in the interrupt itself. It assumed
 * recursive interrupts are not used, so suspend / resume calls are balanced.
 * So to prevent enabling nested interrupts to as a side effect of this interrupt
 * suspend / resume is a bit a puzzle....
 *
 * The only point in code where an interrupt enabled flag can be seen set, is the
 * first call to suspend from the mainloop. This is used to set the stored interrupt
 * value, by default it is off, unless detected by first main call to suspend,
 * subsequent calls have the interrupt flag disabled, so the interruptCounter can be
 * safely updated. If an interrupt is taken between __isflag_int_enabled and the
 * disable interrupt the interruptCount will still be 0 by the assumption that
 * interrupts use balanced calls and don't enable nested interrupts.
 *
 * Note: this is created for testing sending in tx interrupts. For releases, just 
 * don't do that and only guard the rx queue in the application.
 */
void pltInterruptSuspend(void)
{
	if (__isflag_int_enabled()) {
		asm SEI;
		interruptState = 1;
	}
	interruptCount++;
}

/*
 * reset and restore the interrupt state.
 */
void pltInterruptResume(void)
{
	if (--interruptCount == 0 && interruptState)
	{
		un8 tmp = interruptState;
		interruptState = 0;
		if (tmp)
			asm CLI;
	}
}

veBool pltIsTick50ms(void)
{
	if (!tick)
		return veFalse;

	tick = veFalse;
	return veTrue;
}

void veWait(un32 microseconds)
{
	un16 ticks;
	un16 start;

	ticks = microseconds / 200;
	if ((microseconds - 200 * ticks) != 0)
		ticks++;
	
	/* the start value should be exact or it is off by 50ms */
	pltInterruptSuspend();
	start = us200 + RTCCNT;
	if (RTCSC_RTIF)
		start += 250;
	pltInterruptResume();

	/* here no interrupt disable is needed, since it directly exists after the interrupt */
	while ((us200 + RTCCNT - start) < ticks);
}

__interrupt void timer50msInterrupt(void)
{
	tick = veTrue;
	ms += 50;
	us200 += 250;
	RTCSC_RTIF = 1;
}
