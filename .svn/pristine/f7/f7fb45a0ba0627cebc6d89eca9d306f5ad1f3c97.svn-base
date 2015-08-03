#include <windows.h>
#include <stdio.h>
#include <velib/platform/plt.h>
#include <velib/utils/ve_assert.h>

static HANDLE interruptMutex;
static HANDLE wakeup;

static DWORD main_thread_id;

static int interruptsDisabledCounter = 1;

/*
 * Initialises the mutex / semaphores to emulated interrupt behaviour
 * The mutex is owned initially, iow only threads run after pltInterruptEnable.
 */
void pltInitCtx(void)
{
	interruptMutex = CreateMutex(NULL, TRUE, NULL);
	wakeup = CreateEvent(0, FALSE, FALSE, 0);
	main_thread_id = GetCurrentThreadId();
}

/* Enter the low power IDLE mode, and resume on interrupt */
void pltIdleMode(void)
{
	veAssert(interruptsDisabledCounter == 0);
	WaitForSingleObject(wakeup, INFINITE);
}

void pltInterruptBegins(un8 level)
{
	if (!level)
		return;

	pltInterruptSuspend();
}

void pltInterruptEnds(un8 level)
{
	if (!level)
		return;

	pltInterruptResume();
	SetEvent(wakeup);
}

void pltInterruptEnable(void)
{
	pltInterruptResume();
}

void pltInterruptSuspend(void)
{
	WaitForSingleObject(interruptMutex, INFINITE);
	interruptsDisabledCounter++;
}

void pltInterruptResume(void)
{
	interruptsDisabledCounter--;
	veAssert(interruptsDisabledCounter >= 0);
	ReleaseMutex(interruptMutex);
}

void pltInterruptAssertDisabled(int level)
{
	VE_UNUSED(level);

	veAssert(interruptsDisabledCounter > 0);
}
