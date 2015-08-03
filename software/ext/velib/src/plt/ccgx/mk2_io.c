#include <velib/velib_inc_mk2.h>

#include <velib/base/compiler.h>
#include <velib/mk2/hal.h>
#include <velib/utils/ve_logger.h>

#include <stdio.h>
#include <stdlib.h>

static FILE* ioMk2Reset;
static FILE* ioVebusStandby;

void mk2IoInit(void)
{
	if ((ioMk2Reset = fopen("/sys/class/gpio/gpio155/value", "w")) == NULL)
	{
		printf("Cannot open mk2 reset line.\n");
		exit(1);
	}

	if ((ioVebusStandby = fopen("/sys/class/gpio/gpio153/value", "w")) == NULL)
	{
		printf("Cannot open vebus standby line.\n");
		exit(1);
	}

	mk2EnsureVebusPower(veFalse);
}

void mk2HoldInReset(veBool inReset)
{
	if (inReset)
		logI("mk2", "turn mk2 off");
	else
		logI("mk2", "turn mk2 on");

	if (fputc(inReset ? '0' : '1', ioMk2Reset) == EOF) {
		printf("Cannot control mk2 reset line.\n");
		exit(1);
	}
	fflush(ioMk2Reset);
}

void mk2EnsureVebusPower(veBool power)
{
	static int last = -1;

	/* Don't control power if a BMS or DMC is present */
	if (power && !vebusCanBeControlled()) {
		logE("power", "trying to control Ve.Bus power when BMS/DMC is present");
		return;
	}

	if (last == power)
		return;
	last = power;

	if (power)
		logI("mk2", "turn vebus power on");
	else
		logI("mk2", "no longer forcing vebus power to be on");

	if (fputc(power ? '0' : '1', ioVebusStandby) == EOF) {
		printf("Cannot control vebus standby line.\n");
		exit(1);
	}
	fflush(ioVebusStandby);
}
