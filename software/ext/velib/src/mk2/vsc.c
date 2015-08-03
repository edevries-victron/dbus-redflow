#include <string.h>

#include <velib/velib_inc_mk2.h>

#include <velib/base/base.h>
#include <velib/base/ve_string.h>
#include <velib/io/ve_stream.h>
#include <velib/mk2/mk2_strings.h>
#include <velib/mk2/values_float.h>
#include <velib/mk2/vsc.h>
#include <velib/utils/ve_logger.h>

typedef long long sn64;

typedef double (*NormalizeFunc)(VebusVsc const* vsc, veBool write, double data);
static double normalizeDcVoltage(VebusVsc const* vsc, veBool write, double data);
static double normalizeDcCurrent(VebusVsc const* vsc, veBool write, double data);
static double normalizeAcVoltage(VebusVsc const* vsc, veBool write, double data);
static double normalizeAcCurrent(VebusVsc const* vsc, veBool write, double data);
static double normalizeAcPower(VebusVsc const* vsc, veBool write, double data);

static NormalizeFunc const normalizeFunc[] =
{
	/* VE_U_NR */			NULL,
	/* VE_U_V_DC */			normalizeDcVoltage,
	/* VE_U_A_DC */			normalizeDcCurrent,
	/* VE_U_V_AC */			normalizeAcVoltage,
	/* VE_U_A_AC */			normalizeAcCurrent,
	/* VE_U_W */			normalizeAcPower,
	/* VE_U_HZ */			NULL,
	/* VE_U_MSEC */			NULL,
	/* VE_U_DSEC */			NULL,
	/* VE_U_SEC */			NULL,
	/* VE_U_MIN */			NULL,
	/* VE_U_PREC */			NULL,
	/* VE_U_AH */			NULL,
	/* VE_U_FLAG */			NULL,
	/* VE_U_DFLAG */		NULL,
	/* VE_U_FLAGS */		NULL,
	/* VE_U_PREC_FRAC */	NULL,
	/* VE_SELECT */			NULL,
	/* VE_U_INVALID */		NULL
};

/// Delphi round (towards even)
static double dround(double value)
{
	sn64 intpart;
	int sign = (value < 0)? -1 : 1;
	double absval = value * sign;

	intpart = (sn64) absval;
	value = absval - intpart;
	if (value > 0.5 || value == 0.5 && (intpart & 1) == 1)
		intpart++;

	return (double) (intpart * sign);
}

/* correct for the number of cells */
static double normalizeDcVoltage(VebusVsc const* vsc, veBool write, double data)
{
	/*
	 * VS2 reuses the low voltage values to indicate different usage modes.
	 * These must not be normalized!!
	 */
	if (write) {
		if (data >= 2.56)
			return data * dround(VSC_10080_CELL_SYS / mk2GetNoc(vsc->dev));
	} else if (data >= 256) {
		return dround(data * mk2GetNoc(vsc->dev)) / VSC_10080_CELL_SYS;
	}
	return data;
}

/* correct for the number of cells */
static double normalizeDcCurrent(VebusVsc const* vsc, veBool write, double data)
{
	return (write ? data * mk2GetNoc(vsc->dev) : dround(data / mk2GetNoc(vsc->dev)));
}

static double normalizeAcVoltage(VebusVsc const* vsc, veBool write, double data)
{
	/* European */
	if (!mk2IsUS(vsc->dev))
		return data;

	/* US model */
	if (write)
		return dround(data * DEFAULT_VOLTAGE_EU / DEFAULT_VOLTAGE_US);
	return dround(data * DEFAULT_VOLTAGE_US / DEFAULT_VOLTAGE_EU);
}

static double normalizeAcPower(VebusVsc const* vsc, veBool write, double data)
{
	return (write ? data * vsc->workaRound : data / vsc->workaRound);
}

/**
 * Note the factor originates from VEConfigure to workaround a rounding problem.
 * y = round(x*factor)
 * z = round(y/factor)
 * The problem is that z is not necessarily equal to X.
 * However if factor is bigger than 1 then z is always equal to x.
 * With magic factor DEFAULT_VOLTAGE_US * info->workaRound / DEFAULT_VOLTAGE_EU
 * will be > 1.
 */
static double normalizeAcCurrent(VebusVsc const* vsc, veBool write, double data)
{
	if (mk2IsUS(vsc->dev))
	{
		if (write)
			return dround(data * DEFAULT_VOLTAGE_US * vsc->workaRound) / DEFAULT_VOLTAGE_EU;
		return dround(data * DEFAULT_VOLTAGE_EU) / DEFAULT_VOLTAGE_US * vsc->workaRound;
	}

	/* European */
	return (write ? data * vsc->workaRound : data / vsc->workaRound);
}

static void veOutDoubleLe(double value)
{
#if 0
	veOutUn32Le(((un32*) &value)[1]);
	veOutUn32Le(((un32*) &value)[0]);
#else
	veOutUn32Le(((un32*) &value)[0]);
	veOutUn32Le(((un32*) &value)[1]);
#endif
}

static double veInDoubleLe(void)
{
	double value;
#if 0
	((un32*) &value)[1] = veInUn32Le();
	((un32*) &value)[0] = veInUn32Le();
#else
	((un32*) &value)[0] = veInUn32Le();
	((un32*) &value)[1] = veInUn32Le();
#endif
	return value;
}

VebusVscErrNo vebusVscOpenStream(VebusVsc* vsc, struct VebusDeviceS* dev, VeStream* stream, un8 *data, un16 size)
{
	vsc->dev = dev;
	veInStreamInit(stream, data, size);
	veInMove(0);

	// get the vsc footer to locate the vsd footer
	vsc->vscFooter = veInBytesLeft() - VSC_FOOTER_LEN;
	if (vsc->vscFooter <= 0)
		return VE_VSC_FILE_TOO_SHORT;

	// get the vsc footer
	veInMove(vsc->vscFooter);
	vsc->vsiBegin = veInUn32Le();
	if (veInUnderflow())
		return VE_VSC_VSD_END_NOT_WITHIN_FILE;
	vsc->vsiFooter = vsc->vscFooter - VSI_FOOTER_LEN;

	veInMove(vsc->vscFooter + 8);
	if (memcmp(veInPtr(), "VECombinedfile", sizeof("VECombinedfile") - 1) != 0)
		return VE_VSC_NO_COMBINED_FILE;

	if (vebusVscVersion(vsc) != 1)
		return VE_VSC_VERSION_UNKNOWN;

	// Check the vsd header.
	vsc->vsdFooter = vsc->vsiBegin - VSD_FOOTER_LEN;
	veInMove(vsc->vsdFooter + 4);
	if (memcmp(veInPtr(), "VEDatafile", sizeof("VEDatafile") - 1) != 0)
		return VE_VSC_NO_DATA_FILE;

	if (vebusVsdVersion(vsc) < 7 || vebusVsdVersion(vsc) > 8)
		return VE_VSD_VERSION_UNKNOWN;

	/*
	 * Note this was already anticipated for version 8, but didn't make it in the
	 * release of VEConfigure. It was already in the vgr though. The website
	 * corrects for it!.
	 */
	vsc->workaRound = (vebusVsdVersion(vsc) >= 8 ? 10 : 1);

	return VE_VSC_OK;
}

un32 vebusVscVersion(VebusVsc const* vsc)
{
	veInMove(vsc->vscFooter + 4); /* skip vsd len */
	return veInUn32Le();
}

un32 vebusVsdVersion(VebusVsc const* vsc)
{
	veInMove(vsc->vsdFooter);
	return veInUn32Le();
}

void vebusVscReadData(VebusVsc const* vsc, un16* data, un8 n)
{
	un8 id;
	double value;
	VebusUnit unit;

	veInMove(4);
	do {
		id = veInUn8();
		value = veInDoubleLe();

		if (id >= 0x80) {
			vebusLocalSetFlag(vsc->dev, id - 0x80 + VEBUS_FLAGS_OFFSET, value != 0);
			continue;
		}

		unit = vebusGetUnit(id, VE_UNITS_VSC);
		if (unit == VE_U_INVALID)
			continue;

		if (normalizeFunc[unit]) {
			//logI("raw", "%d %Lf", (int) id, value);
			value = normalizeFunc[unit](vsc, veFalse, value);
			//logI("normalized", "%d %Lf", (int) id, value);
		}

		if (id >= VEBUS_FLAGS_OFFSET) {
			logI("flag", "%d %s %d", (int) id, vebusGetSettingName(id), value != 0);
			vebusLocalSetFlag(vsc->dev, id, value != 0);
			continue;
		}

		if (id == 12)
			logI("debug", "");

		value /= vebusSettingScale(vsc->dev, id, VE_UNITS_VSC);
		value -= vebusSettingOffset(vsc->dev, id, VE_UNITS_VSC);

		if (id < n)
			data[id] = (un16) dround(value);
	} while (veStreamIn->index < vsc->vsdFooter);
}

void vebusVscAddSetting(VebusVsc const* vsc, VebusDevice* dev, un8 id)
{
	VebusUnit unit = vebusGetUnit(id, VE_UNITS_VSC);
	double value = vebusGetSettingValue(dev, id, VE_UNITS_VSC);

	veOutUn8(id);
	if (normalizeFunc[unit]) {
		//logI("before", "%d %Lf", (int) id, value);
		value = normalizeFunc[unit](vsc, veTrue, value);
	}
	//logI("raw", "%d %Lf", (int) id, value);
	veOutDoubleLe(value);
}

void vebusVscAddFlag(VebusVsc const* vsc, VebusDevice* dev, un8 id)
{
	double value = vebusGetFlag(dev, id);

	//logI("raw", "%d %Lf", (int) id, value);
	veOutUn8((un8) id + 0x80);
	veOutDoubleLe(value);
}


void mk2VscAddScale(VebusDevice* dev, un8 id)
{
	veOutUn8(id);
	veOutSn32Le(vebusGetSettingRawDefault(dev, id));
	veOutSn32Le(vebusGetSettingRawMinimum(dev, id));
	veOutSn32Le(vebusGetSettingRawMaximum(dev, id));
	veOutSn32Le(vebusSettingOffset(dev, id, VE_UNITS_MULTI));
	veOutDoubleLe(vebusSettingScale(dev, id, VE_UNITS_MULTI));
}

void mk2VscAddScaleFlag(VebusDevice* dev, un16 nr)
{
	veOutUn8((un8) nr + 0x80);
	veOutSn32Le(vebusGetFlagRawDefault(dev, (un8)nr));
	veOutSn32Le(0);	/* min */
	veOutSn32Le(1); /* max */
	veOutSn32Le(0);
	veOutDoubleLe(vebusSupportedVariable(dev, (un8)nr));
}

static void addCheckSum(un16 from)
{
	un16 n;
	un8 sum = 0;

	for (n = from; n < veStreamOut->index; n++)
		sum += veStreamOut->buffer[n];
	sum = ~sum + 1;
	veOutUn8(sum);
}

static veBool addSetting(struct VebusDeviceS *dev, un16 id)
{
	VebusUnit unit;

	if (!vebusSupportedVariable(dev, id))
		return veFalse;

	unit = vebusGetUnit(id, VE_UNITS_VSC);
	if (unit == VE_U_DFLAG || unit == VE_U_FLAGS)
		return veFalse;

	return veTrue;
}

void vebusCreateVsc(VeStream* stream, struct VebusDeviceS *dev, un8* buf, un16 len)
{
	un16 nr;
	un16 vsdLen;
	un16 vscLen;
	VebusVsc vsc;

	vsc.workaRound = 10;
	vsc.dev = dev;
	veOutStreamInit(stream, buf, len);

	veOutUn32Le(dev->version);

	/* Add the flags values */
	for (nr = VEBUS_FLAGS_OFFSET; nr < VEBUS_FLAGS_ALLOCATED + VEBUS_FLAGS_OFFSET; nr++)
		if (addSetting(dev, nr))
			vebusVscAddFlag(&vsc, dev, (un8) nr);

	/* The data part (vsd) of the file */
	for (nr = 0; nr < VEBUS_SETTINGS_COUNT; nr++)
		if (addSetting(dev, nr))
			vebusVscAddSetting(&vsc, dev, (un8) nr);

	/* The vsd footer */
	veOutUn32Le(VSD_FILE_VERSION);
	veOutBuf("VEDatafile", sizeof("VEDatafile") - 1);
	addCheckSum(0);
	vsdLen = stream->index;

	/* The info part of the file */
	veOutUn32Le(dev->version);

	/* Add the flags info */
	for (nr = VEBUS_FLAGS_OFFSET; nr < VEBUS_FLAGS_OFFSET + VEBUS_FLAGS_ALLOCATED; nr++)
		if (addSetting(dev, nr))
			mk2VscAddScaleFlag(dev, nr);

	for (nr = 0; nr < VEBUS_SETTINGS_COUNT; nr++)
		if (addSetting(dev, nr))
			mk2VscAddScale(dev, (un8) nr);

	/* And its footer */
	veOutUn32Le(VSI_FILE_VERSION);
	veOutBuf("VESettingfile", sizeof("VESettingfile") - 1);
	addCheckSum(vsdLen);
	vscLen = stream->index - vsdLen;

	/* footer of the whole file */
	veOutUn32Le(vsdLen);
	veOutUn32Le(VSC_FILE_VERSION);
	veOutBuf("VECombinedfile", sizeof("VECombinedfile") - 1);
	addCheckSum(0);
}

/* This is for debug only, normally these values are already obtained from the device */
void vebusVscReadSettingInfo(VebusVsc const* vsc, VebusSettingInfo* info, un8 n)
{
	double scale;

	/* not all scales are per definition in the file, clear all out */
	memset(info, 0, n * sizeof(VebusSettingInfo));
	veInMove(vsc->vsiBegin + 4);

	do {
		un8 id = veInUn8();

		if (id >= VEBUS_FLAGS_OFFSET) {
			sn32 def, min, max, offset;

			def = veInSn32Le();
			min = veInSn32Le();
			max = veInSn32Le();
			offset = veInSn32Le();
			scale = veInDoubleLe();
			logI("vsc", "flag %d = %d [%d..%d] (val+%d)*%X", id, def, min, max, offset, (int) dround(scale));
			continue;
		} else if (id >= n) {
			logI("vsc", "unknown setting %d", (int) id);
			veInFixedString(24);
			continue;
		}
		/* store scale information */
		info[id].defaultValue = veInSn32Le();
		info[id].minimum = veInSn32Le();
		info[id].maximum = veInSn32Le();
		info[id].offset = veInSn32Le();
		scale = veInDoubleLe();
		if (scale < 1)
			scale = 1 / -scale;
		info[id].scale = (sn16) scale;

		logI("vsc", "%d", (int) id);
	} while (veStreamIn->index < vsc->vsiFooter);
}
