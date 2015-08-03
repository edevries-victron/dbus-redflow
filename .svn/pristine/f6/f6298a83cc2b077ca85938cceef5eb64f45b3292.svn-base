#include <stdlib.h>

#include <velib/io/ve_remote_device_item.h>
#include <velib/utils/ve_assert.h>

#include "ve_remote_device_internal.h"

static veBool prepareOut(VeRemoteDev *tunnel)
{
	VeItemRemoteDev *dbusTunnel = (VeItemRemoteDev *) tunnel;
	size_t bufSize = 255;
	un8 *buf = malloc(bufSize);

	if (!buf)
		return veFalse;

	veOutStreamInit(&dbusTunnel->stream, buf, bufSize);
	return veTrue;
}

static veBool sendOutput(VeRemoteDev *tunnel)
{
	VeItemRemoteDev *dbusTunnel = (VeItemRemoteDev *) tunnel;
	VeVariant var;

	var.type.tp = VE_HEAP;
	var.type.len = dbusTunnel->stream.index;;
	var.value.CPtr = dbusTunnel->stream.buffer;

	if (tunnel->role == VE_REMOTE_DEV_ROLE_SERVICE || tunnel->role == VE_REMOTE_DEV_ROLE_SERVICE_PROXY)
		veItemOwnerSet(dbusTunnel->item, &var);
	else
		veItemSet(dbusTunnel->item, &var);

	/* Free the item directly, to make sure change events are always generated */
	var.type.tp = VE_HEAP;
	var.value.Ptr = NULL;
	veItemLocalSet(dbusTunnel->item, &var);

	return dbusTunnel->stream.error;
}

static veBool itemSetValueCb(struct VeItem *item, void *ctx, VeVariant *var)
{
	VeStream stream;
	VeRemoteDev *tunnel = (VeRemoteDev *) ctx;
	veAssert(var->type.tp == VE_HEAP);
	VE_UNUSED(item);

	veInStreamInit(&stream, var->value.Ptr, var->type.len);
	veRemoteDevRxData(tunnel);

	return veTrue;
}

static void itemChangedCallback(struct VeItem *item)
{
	VeStream stream;
	VeRemoteDev *tunnel = (VeRemoteDev *) veItemCtx(item)->ptr;
	VeVariant var;

	/* deserialize it to send the contained data to rxData */
	veItemLocalValue(item, &var);
	veInStreamInit(&stream, var.value.Ptr, var.type.len);
	veRemoteDevRxData(tunnel);
}

void veItemRemoteDevInit(VeItemRemoteDev *dev, VeItem *item, VeRemoteDevRole role)
{
	VeVariant var;

	veRemoteDevInit(&dev->remoteDev, prepareOut, sendOutput, role);

	if (role == VE_REMOTE_DEV_ROLE_SERVICE || role == VE_REMOTE_DEV_ROLE_SERVICE_PROXY) {
		veItemSetSetter(item, itemSetValueCb, dev);
	} else {
		veItemSetChanged(item, itemChangedCallback);
		veItemCtx(item)->ptr = dev;
	}

	dev->item = item;

	var.type.tp = VE_HEAP;
	var.value.Ptr = NULL;
	veItemLocalSet(item, &var);
}
