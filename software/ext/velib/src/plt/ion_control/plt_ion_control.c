#include <C164CxSx.H>
#include <intrins.h>
#include <stdio.h>

#include <velib/platform/plt.h>
#include <velib/canhw/canhw.h>
#include <velib/utils/ve_assert.h>

#define T3CON_T3M_TIMER			(0 << 3)
#define T3CON_T3M_COUNTER		(1 << 3)
#define T3CON_T3R				(1 << 6)

#define T3_COUNT				62500
#define T3_RELOAD				(0x10000 - T3_COUNT)
#define T3INT					0x23

/* uart */
#define RS232					1

#define SYNC_8BIT				0x0000
#define ASYNC_8BIT_NOPARITY		( 1 << 0)
#define ASYNC_7BIT_PARITY		((1 << 0) | (1 << 1))
#define ASYNC_9BIT_NOPARITY		(1 << 2)
#define ASYNC_8BIT_WAKEUP		((1 << 0) | (1 << 2))
#define ASYNC_8BIT_PARITY		((1 << 0) | (1 << 1) | (1 << 2))

#define NO_PARITY				0x0000
#define PARITY					(1 << 5)

#define EVEN					0x0000
#define ODD						(1 << 12)

#define STOP1					0x0000
#define STOP2					(1 << 3)
#define BAUD57600				0x00a

sbit  UART_EXCH_PIN = P1L^7;

static un32 ticks800ns;
static veBool isTick50ms;
static un16 ms;

void uartInit(void)
{
	S0CON = ASYNC_8BIT_NOPARITY | STOP1;
	
	// ASC baudrate reload register
	S0BG = BAUD57600;
	
	P3 |= 0x0400; /*  set P3.10 output latch (TXD0) */
	DP3 |= 0x0400; /* set P3.10 direction control (TXD0 output) */
	DP3 &= 0xF7FF; /* reset P3.11 direction control (RXD0 input) */

	UART_EXCH_PIN = RS232;
	
	/*  enable baudrate generator */
	S0R = 1;
}

void veAssertFailedNum(char const* file, un16 line)
{
	IEN = 0;

	printf("assertion failure at %s line %d\r\n", file, line);
	for (;;) {
		_srvwdt_();
	}
}

/* Non-Maskable Interrupt */
void NMI_trap(void) interrupt 0x02
{
	veAssert(veFalse);
}

/* Stack Overflow Interrupt */
void STKOF_trap(void) interrupt 0x04
{
	veAssert(veFalse);
}

/* Stack Underflow Interrupt */
void STKUF_trap(void) interrupt 0x06
{
	veAssert(veFalse);
}

/*
 * Class B Hardware Trap:
 *   ->  Undefined Opcode
 *   ->  Protected Instruction Fault
 *   ->  Illegal Word Operand Access
 *   ->  Illegal Instruction Access
 *   ->  Illegal External Bus Access
 */
void Class_B_trap(void) interrupt 0x0A 
{
	veAssert(veFalse);
}

void GT1_viIsrTmr3(void) interrupt T3INT
{
	T3 = T3_RELOAD;
	ticks800ns += T3_COUNT;
	isTick50ms = veTrue;
	ms += 50;
}

/*
 * freq t3 = fcpu / (8 x 2 ^ T3I) = 20Mhz / 8 / 2 = 1.25Mhz
 * 1 tick = 800ns. 50ms / 800ns = 50,000,000ns / 800 = 62500.
 */
void timerInit(void)
{
	T3 = T3_RELOAD;
	T3IC = 0x006D;
	T3CON = T3CON_T3R | T3CON_T3M_TIMER | 1 /* T3I */;
}

#define WDTIN			1

/* Note: the watchdog cannot be disabled! */
void watchdogInit(void)
{
	/* 
	 * Pwdt = 2 ^(1 + WDTPRE + WDTIN * 6) x ( 2^16 - WDTREL x 2^8) / fcpu
	 *
	 * WDTIN = 1, WDTPRE = 0, WDTREL = 0
	 *		= 2 ^(1 + 0 + 1 * 6) x ( 2^16 - 0 x 2^8) / fcpu
	 *		= 2 ^ 7 x 2^16 / fcpu = 128 * 65,280 / 20Mhz = +/- 0.42 s
	 */
	WDTCON = WDTIN;
}

void pltInit(void)
{
	IEN = 0;
	ADCIC = 0;

	uartInit();
	watchdogInit();

	printf("starting\n");

	timerInit();
	veCanInit();
	_srvwdt_();
}

un16 pltGetCount1ms(void)
{
	un16 ret;

	pltInterruptSuspend();

	ret = T3;
	if (T3IR)
		ret = T3_COUNT;
	else
		ret -= T3_RELOAD;

	ret /= T3_COUNT / 50;
	ret += ms;

	pltInterruptResume();

	return ret;
}

un8 veRand(void)
{
	return T3;
}

un32 getTick800ns(void)
{
	un32 ret;

	pltInterruptSuspend();

	ret = T3;
	if (T3IR)
		ret = T3_COUNT;
	else
		ret -= T3_RELOAD;
	ret += ticks800ns;

	pltInterruptResume();

	return ret;
}

veBool pltIsTick50ms(void)
{
	if (!isTick50ms)
		return veFalse;

	isTick50ms = veFalse;
	_srvwdt_();

	return veTrue;
}

void veWait(un32 microseconds)
{
	un32 ticks;
	un32 start;

	ticks = microseconds * 1000 / 800;
	start = getTick800ns();

	while ((getTick800ns() - start) < ticks);
}

static sn8 interruptCounter;

void pltInterruptEnable(void)
{
	IEN = 1;
}

void pltInterruptSuspend(void)
{
	_bfld_(PSW, 0xF000, 0xF000);	/* disable interrupts */
	_nop_();						/* avoid pipeline effect */
	_nop_();
	interruptCounter++;
}

void pltInterruptResume(void)
{
	if (--interruptCounter == 0)
		_bfld_(PSW, 0xF000, 0x0000);
}