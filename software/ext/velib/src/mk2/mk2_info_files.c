#include <stdio.h>

#include <velib/base/ve_string.h>
#include <velib/mk2/vebus_device.h>

/**
 * The settings and ram scaling must be obtained from the vebus device,
 * which takes some time. To speed up initialisation they might be stored.
 * Depending on the device type this can be done in flash / EEPROM etc.
 * This is the filesystem version as used by pc's.
 */

static char *dir = "";

static FILE* openRamFile(un32 version, char const* mode)
{
	char buf[255];
	ve_snprintf(buf, sizeof(buf), "%s%d.ram", dir, version);
	return fopen(buf, mode);
}

void mk2InfoRamStore(un32 version, VebusRamInfo* info, un16 numVars)
{
	FILE *fh;

	if (!(fh = openRamFile(version, "wb")))
		return;

	fwrite(info, sizeof(VebusRamInfo), numVars, fh);
	fclose(fh);
}

veBool mk2InfoRamLoad(un32 version,  VebusRamInfo* info, un16 numVars)
{
	FILE *fh;

	if (!(fh = openRamFile(version, "rb")))
		return veFalse;

	if (fread(info, sizeof(VebusRamInfo), numVars, fh) < numVars) {
		fclose(fh);
		return veFalse;
	}

	fclose(fh);
	return veTrue;
}

static FILE* openSettingsFile(un32 version, char const* mode)
{
	char buf[255];
	ve_snprintf(buf, sizeof(buf), "%s%d.set", dir, version);
	return fopen(buf, mode);
}

void mk2InfoSettingsStore(un32 version, VebusSettingInfo* info, un16 numSettings)
{
	FILE *fh;

	if (!(fh = openSettingsFile(version, "wb")))
		return;

	fwrite(info, sizeof(VebusSettingInfo), numSettings, fh);
	fclose(fh);
}

veBool mk2InfoSettingsLoad(un32 version, VebusSettingInfo* info, un16 numSettings)
{
	FILE *fh;

	if (!(fh = openSettingsFile(version, "rb")))
		return veFalse;

	if (fread(info, sizeof(VebusSettingInfo), numSettings, fh) < numSettings) {
		fclose(fh);
		return veFalse;
	}

	fclose(fh);
	return veTrue;
}
