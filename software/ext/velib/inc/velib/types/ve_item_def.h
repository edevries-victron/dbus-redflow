#ifndef _VELIB_TYPES_VE_ITEM_DEF_H_
#define _VELIB_TYPES_VE_ITEM_DEF_H_

/*
 * Note: only include this header when actually declaring VeItems, use
 * ve_item.h / struct VeItem otherwise.
 */

#include <velib/types/ve_item.h>

typedef struct VeItem {
	char *id;

	struct VeItem *oldest; /* child */
	struct VeItem *youngest; /* child */
	struct VeItem *younger; /* brother */
	struct VeItem *older;  /* brother */

	VeVariant variant;
	VeVariant min;
	VeVariant max;
	VeVariant defaultValue;

	VeItemValueFmt *toString;
	struct VeItem *parent;

	un8 validTimeReset;
	un8 validTime;

	void const *toStringCtx;
	VeItemValueChanged *changedFun;
	VeItemValueChanged *minChangedFun;
	VeItemValueChanged *maxChangedFun;
	VeItemValueChanged *defaultChangedFun;
	VeItemChildAdded *childAddedFun;
	VeItemAboutToBeRemoved *beforeRemoveFun;

	/* context of the owner */
	VeItemCtx ctx;

	VeItemGetterFun *getFun;
	VeItemSetterFun *setFun;
	VeItemGetDescription *getDescr;

	void *setCtx;

	/* internally used for dbus backend, leave untouched */
	void *dbus;

	un16 itemAllocated : 1;
	un16 idAllocated : 1;
} VeItem;

#endif

