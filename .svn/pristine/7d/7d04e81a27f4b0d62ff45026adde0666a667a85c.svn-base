/**
 * Rather straightforward, getting values of the udf/vup, with the only
 * reminder what they mean.
 *
 * udf - Update Device Function
 *   Actually performing the update, only has a version and some flags.
 *
 * vup - Victron Updatable
 *	Something updatable, but there are no strict restrictions on what it is.
 *	It must have a product id and preferably a version.
 *
 * The vup and udf can be the same (CAN enabled device updating itself).
 * Or distinct (a CAN BMV interface updating the BMV itself).
 *
 * @note
 *	There should only one udf per product.
 *
 * @note
 *	A single udf can have several vup's.
 */

#include <string.h> // memset

#include <velib/velib_inc_J1939.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_function.h>
#include <velib/utils/ve_assert.h>
#include <velib/vecan/dev_udf.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>


void veDevUdfUpdate(J1939FuncTp* veDev)
{
	veAssert(veDev);

	// note this request all boot registers at once!
	if (veDev->udf.flags & VE_DEV_UDF_REQ)
		if (veRegRequestMaskWhenOnBus(veDev, VE_REG_PRODUCT_ID, 0xFF00))
			veDev->udf.flags &= ~VE_DEV_UDF_REQ;
}

VeDevVup* veDevUdfFindVup(J1939FuncTp* veDev, un8 instance)
{
	VeDevVup* vup;
	veAssert(veDev);

	vup = veDev->udf.vup;
	while (vup != NULL)
	{
		if (vup->instance == instance)
			return vup;

		vup = vup->next;
	}
	return NULL;
}

/// seperate function to add, might want to sort some day.
void veDevUdfAddVup(J1939FuncTp* veDev, VeDevVup* vup)
{
	veAssert(veDev);
	veAssert(vup);
	veAssert(vup->next == NULL);

	if (veDev->udf.vup == NULL)
		veDev->udf.vup = vup;
	else
	{
		vup->next = veDev->udf.vup;
		veDev->udf.vup = vup;
	}
}

/**
 * Allocates storage for a vup (Victron Updatable Device).
 *
 * @note There can be multiple updatable device in or behind a
 * single CANbus device.
 */
VeDevVup* veDevUdfGetVup(J1939FuncTp* veDev, un8 instance)
{
	VeDevVup* vup;

	vup = veDevUdfFindVup(veDev, instance);
	if (vup != NULL)
		return vup;

	vup = veDevVupAlloc();
	if (vup == NULL)
		return NULL;

	memset(vup, 0, sizeof(VeDevVup));
	vup->instance = instance;

	veDevUdfAddVup(veDev, vup);

	return vup;
}

/**
 * Adds the udf / vup info to the device
 */
void veDevUdfDataVeRegs(VeEvent ev)
{
	J1939FuncTp* veDev;
	VeDevVup* vup;
	un16 prodId;
	un32 version;
	un8 instance;
	un8 flags;

	if (ev != VE_EV_VREG_RX)
		return;

	veDev = j1939FuncByNad(n2kMsgIn->msgId.p.src);
	if (veDev == NULL)
		return;

	switch (veRegRxId)
	{
	case VE_REG_UDF_VERSION:
		veDev->udf.udfVersion = veInUn24Le();
		veDev->udf.udfFlags = veInUn8();
		if (!veStreamIn->error)
			veDev->udf.flags |= VE_DEV_UDF_VER_VALID;
		break;

	case VE_REG_PRODUCT_ID:
		instance = veInUn8();
		prodId = veInUn16Le();
		flags = veInUn8();

		if (veStreamIn->error)
			return;

		if ((vup = veDevUdfGetVup(veDev, instance)) == NULL)
			return;

		vup->prodId = prodId;
		vup->prodFlags = flags;
		vup->flags |= VE_DEV_VUP_PROD_ID;
		break;

	case VE_REG_APP_VER:
		instance = veInUn8();
		version = veInUn24Le();

		if (veStreamIn->error)
			return;

		if ((vup = veDevUdfGetVup(veDev, instance)) == NULL)
			return;

		vup->appVersion = version;
		vup->flags |= VE_DEV_VUP_VER;

		break;

	case VE_REG_APP_VER_MIN:
		instance = veInUn8();
		version = veInUn24Le();

		if (veStreamIn->error)
			return;

		if ((vup = veDevUdfGetVup(veDev, instance)) == NULL)
			return;

		vup->appVersionMin = version;
		vup->flags |= VE_DEV_VUP_VER_MIN;
		break;

	case VE_REG_GROUP_ID:
		veDev->udf.group = veInUn8();
		veDev->udf.flags |= VE_DEV_GROUP_VALID;
		break;

	default:
		break;
	}
}
