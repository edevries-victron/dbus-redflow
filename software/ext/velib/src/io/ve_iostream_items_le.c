#include <velib/io/ve_stream_items.h>
#include <velib/io/ve_stream_n2k.h>

/**
 * Types/ve_item.c provides basic timeout / onchange / onset
 * functionality around a variant. These routines update these
 * item directly from the nmea 2000 input stream.
 */

/* handle normalizing seperately to keep floats optional */
static void handleDecimals(VeVariant *var, un8 decimals)
{
	if (decimals == 0 || !veVariantIsValid(var))
		return;

#if CFG_WITH_FLOAT
	/* If floating are supported, normalize the unit */
	veVariantToFloat(var);
	var->value.Float /= (float) pow(10.0, decimals);
#else
	/* Otherwise store the decimals places in the variant */
	var->type.len = decimals;
#endif
}

/**
 * Read a variant of type from the instream and normalize the value.
 * e.g. a read of 1200 mV, needs decimals is 3 to make it 1.200 V.
 * If floats are not supported the decimals is stored in the len field
 * of the variant (with is not used for numeric types).
 * When withOutOfRange is set to veTrue, the item is set to invalid when
 * an n2k out of range value is received (MAXVAL - 1).
 */
static veBool veInVariantDecimalsOutOfRangeLe(VeVariant *v, VeDataBasicType type,
											  un8 decimals, veBool withOutOfRange)
{
	v->type.tp = type;
	veInVariantLe(v);

	if (n2kStreamIn.error) {
		veVariantInvalidate(v);
		return veFalse;
	}

	if (withOutOfRange && veVariantIsOutOfRange(v)) {
		veVariantInvalidate(v);
		return veFalse;
	}

	handleDecimals(v, decimals);
	return veTrue;
}

/** See above, default to not interpeting out of range values */
veBool veInVariantDecimalsLe(VeVariant *v, VeDataBasicType type, un8 decimals)
{
	return veInVariantDecimalsOutOfRangeLe(v, type, decimals, veFalse);
}

/**
 * Read a variant of given type and update the item to it.
 * Note: this directly causes on change events, if the value needs to
 * be manupulated, use veInVariantDecimalsLe / veItemOwnerSet.
 */
static void veInItemOutOfRangeLe(struct VeItem *item, VeDataBasicType type,
								 un8 decimals, veBool withOutOfRange)
{
	VeVariant v;

	if (item == NULL)
		return;

	veInVariantDecimalsOutOfRangeLe(&v, type, decimals, withOutOfRange);
	veItemOwnerSet(item, &v);
}

void veInItemLe(struct VeItem *item, VeDataBasicType type, un8 decimals)
{
	veInItemOutOfRangeLe(item, type, decimals, veFalse);
}

void veInN2kDcV(struct VeItem *item)
{
	veInItemLe(item, VE_SN16, 2);
}

void veInN2kDcI(struct VeItem *item)
{
	veInItemLe(item, VE_SN16, 1);
}

void veInN2kDcT(struct VeItem *item)
{
	veInItemOutOfRangeLe(item, VE_UN16, 2, veTrue);
}

void veInJ1939Voltage(struct VeItem *item)
{
	veInItemLe(item, VE_UN16, 0);
}

void veInJ1939Hz(struct VeItem *item)
{
	VeVariant v;

	v.type.tp = VE_UN32;
	v.value.UN32 = veInUn16Le();

	if (!validUN16(v.value.UN32) || n2kStreamIn.error) {
		veVariantInvalidate(&v);
	} else {
		v.value.UN32 = v.value.UN32 * 100000 / 128;
		v.value.UN32 /= 10000;
		handleDecimals(&v, 1);
	}

	veItemOwnerSet(item, &v);
}

void veInJ1939Current(struct VeItem *item)
{
	veInItemLe(item, VE_UN16, 0);
}

void veInJ1939Power(struct VeItem *item)
{
	VeVariant v;

	v.type.tp = VE_SN32;
	v.value.UN32 = veInUn32Le();

	if (!validUN32(v.value.UN32) || n2kStreamIn.error) {
		veItemInvalidate(item);
	} else {
		v.value.SN32 = v.value.UN32 - 2000000000;
		veItemOwnerSet(item, &v);
	}
}
