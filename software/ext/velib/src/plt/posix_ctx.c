#include <pthread.h>
#include <semaphore.h>

#include <velib/platform/plt.h>
#include <velib/utils/ve_assert.h>

static pthread_mutex_t intLevel1 = PTHREAD_MUTEX_INITIALIZER;
static sem_t wakeupSemaphore;
static veBool interruptEnabled;

/*
 * Initialises the mutex / semaphores to emulated interrupt behaviour.
 * Bare bone projects don't need this since the silicon takes care of it.
 *
 * @note
 *	The init functions emaluates a mcu startup, thus with interrupts
 *	disabled. Only after pltInterruptEnabled is called, threads start to
 *  run which are guarded by pltInterruptBegins / pltInterruptEnds.
 */
void pltInitCtx(void)
{
	pthread_mutexattr_t mta;

	pthread_mutexattr_init(&mta);
	pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&intLevel1, &mta);
	pthread_mutex_lock(&intLevel1);
	interruptEnabled = veFalse;

	sem_init(&wakeupSemaphore, 0, 0);
}

/*
 * wake up the mainloop, to prevent spurious wakes this should hold the
 * interrupt mutex.
 */
static void pltIdleExit(void)
{
	int ret, value;

	/*
	 * If the main thread is blocking or would block on the semaphore, prevent that
	 * by incrementing it / set it to a runable state.
	 */
	ret = sem_getvalue(&wakeupSemaphore, &value);
	if (ret == 0 && value <= 0)
		sem_post(&wakeupSemaphore);
}

/*
 * Used by threads, to emulated a single interrupt context.
 * Whenever a non main thread makes unsafe calls / accesses the code
 * should be in between pltInterruptBegins / pltInterruptEnds.
 * The main thread uses pltInterruptSuspend / pltInterruptResume.
 *
 * @note
 *	Level can be used to simulated interrupt being interrupted.
 *	Since this is typically not used, it is not implemented.
 *	At the moment 0 -> main, 1 -> interrupt.
 */
void pltInterruptBegins(un8 level)
{
	if (!level)
		return;

	/* Only one interrupt at a time */
	pthread_mutex_lock(&intLevel1);
}

void pltInterruptEnds(un8 level)
{
	if (!level)
		return;

	pltIdleExit();

	/* Only one "interrupt" at a time  and not during interrupt disabled */
	pthread_mutex_unlock(&intLevel1);
}

void pltInterruptEnable(void)
{
	interruptEnabled = veTrue;
	pthread_mutex_unlock(&intLevel1);
}

void pltInterruptSuspend(void)
{
	/* Disable "interrupts" / block threads */
	pthread_mutex_lock(&intLevel1);
	interruptEnabled = veFalse;
}

void pltInterruptResume(void)
{
	/* allow "interrupts" again */
	interruptEnabled = veTrue;
	pthread_mutex_unlock(&intLevel1);
}

/* Enter the low power IDLE mode, and resume on interrupt */
void pltIdleMode(void)
{
	/*
	 * note: this either wakes up because the semaphore gets incremented or a signal
	 * has been received. Since the signal might be a timer signal, spin to check.
	 */
	sem_wait(&wakeupSemaphore);
}

void pltInterruptAssertDisabled(int level)
{
	VE_UNUSED(level);
	//veAssert(interruptEnabled == veFalse);
}
