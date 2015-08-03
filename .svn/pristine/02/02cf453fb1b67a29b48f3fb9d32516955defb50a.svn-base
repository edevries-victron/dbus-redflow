#include <velib/canhw/canhw_driver.h>
#include <velib/utils/ve_logger.h>

/*
 * These are just stubs to make dynamically loaded CAN drivers use the
 * same calls as when they are statically linked. The shared object
 * loader must call veCanDrvDynInit to check compatibility and setup
 * callbacks. Try to keep this as short as possibe.
 */

static VeCanDriverCallbacks* app;

veBool veCanDrvDynInit(VeCanDriverCallbacks* callbacks)
{
	app = callbacks;

	if (callbacks->version != VE_CAN_DRIVER_API) {
		logE("canhw", "version mismatch %d %d", callbacks->version, VE_CAN_DRIVER_API);
		app = NULL;
		return veFalse;
	}

	return veTrue;
}

void veCanDrvNewGw(VeCanDriver *driver, VeCanGateway *gw)
{
	if (app && app->newGwFun)
		app->newGwFun(driver, gw);
}

J1939Sf* j1939SfRxAlloc(void)
{
	if (app && app->j1939SfRxAllocFun)
		return app->j1939SfRxAllocFun();
	else
		return NULL;
}

veBool n2kIsFp(J1939Id const *id)
{
	if (app && app->n2kIsFpFun)
		return app->n2kIsFpFun(id);
	else
		return veFalse;
}

N2kFp* n2kFpRxAlloc(void)
{
	if (app && app->n2kFpRxAllocFun)
		return app->n2kFpRxAllocFun();
	else
		return NULL;
}

void n2kFpRxFree(N2kFp *msg)
{
	if (app && app->n2kFpRxFreeFun)
		app->n2kFpRxFreeFun(msg);
}

// ERROR
VE_DCL void logE(char const* module, char const* format, ...)
{
	va_list	argp;

	va_start(argp, format);
	app->logE(module, format, argp);
	va_end(argp);
}

// ERROR
VE_DCL void logI(char const* module, char const* format, ...)
{
	va_list	argp;

	va_start(argp, format);
	app->logI(module, format, argp);
	va_end(argp);
}
