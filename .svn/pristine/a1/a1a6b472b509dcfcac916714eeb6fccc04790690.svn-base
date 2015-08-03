#ifndef _VELIB_TYPES_VE_ITEM_H_
#define _VELIB_TYPES_VE_ITEM_H_

#include <velib/types/variant.h>

struct VeItem;
typedef struct VeItem VeItem;

#define VE_MAX_UID_SIZE			256

typedef size_t VeItemValueFmt(VeVariant *var, void const *ctx,
									char *buf, size_t len);

typedef void VeItemForeachFun(struct VeItem *item, void *ctx);
typedef veBool VeItemSetterFun(struct VeItem *item, void *ctx, VeVariant *variant);
typedef void VeItemGetterFun(struct VeItem *item);
typedef void VeItemValueChanged(struct VeItem *item);
typedef void VeItemChildAdded(struct VeItem *childItem);
typedef void VeItemAboutToBeRemoved(struct VeItem *item);
typedef size_t VeItemGetDescription(struct VeItem *item, char *buf, size_t len);

typedef union {
	void *ptr;
	un32 value;
} VeItemCtx;


VE_DCL void					veItemTick(struct VeItem *root);

VE_DCL struct VeItem *		veItemAddChildByUid(struct VeItem *item, char const *uid, struct VeItem *child);
VE_DCL void					veItemAddChild(struct VeItem *parent, struct VeItem *child);
VE_DCL struct VeItem *		veItemAlloc(struct VeItem *parent, char const *id);
VE_DCL struct VeItem *		veItemByUid(struct VeItem *item, char const *uid);
VE_DCL VeItemCtx *			veItemCtx(struct VeItem *item);
VE_DCL void					veItemDefaultValue(struct VeItem *item, VeVariant *defaultValue);
VE_DCL void					veItemDeleteBranch(struct VeItem *item);
VE_DCL size_t				veItemDescription(struct VeItem *item, char *buf, size_t len);
VE_DCL void					veItemDump(struct VeItem *item);
VE_DCL struct VeItem *		veItemFirstChild(struct VeItem *parent);
VE_DCL void					veItemForeach(struct VeItem *item, VeItemForeachFun* fun, void *ctx);
VE_DCL void					veItemForeachChildsFirst(struct VeItem *item, VeItemForeachFun *fun, void *ctx);
VE_DCL struct VeItem *		veItemGetOrCreateUid(struct VeItem *item, char const *uid);
VE_DCL char const *			veItemId(struct VeItem *item);
VE_DCL struct VeItem *		veItemInit(struct VeItem *child, char *id, struct VeItem *parent);
VE_DCL void					veItemInvalidate(struct VeItem *item);
VE_DCL veBool				veItemIsValid(struct VeItem *item);
VE_DCL void					veItemLocalSet(struct VeItem *item, VeVariant *variant);
VE_DCL VeVariant *			veItemLocalValue(struct VeItem *item, VeVariant *var);
VE_DCL void					veItemMax(struct VeItem *item, VeVariant *max);
VE_DCL void					veItemMin(struct VeItem *item, VeVariant *min);
VE_DCL struct VeItem *		veItemNextChild(struct VeItem *child);
VE_DCL veBool				veItemOwnerSet(struct VeItem *item, VeVariant *variant);
VE_DCL struct VeItem *		veItemParent(struct VeItem *item);
VE_DCL struct VeItem *		veItemRoot(struct VeItem *item);
VE_DCL veBool				veItemSet(struct VeItem *item, VeVariant *variant);
VE_DCL veBool				veItemSetAboutToRemoved(struct VeItem *item, VeItemAboutToBeRemoved *fun);
VE_DCL veBool				veItemSetChanged(struct VeItem *item, VeItemValueChanged* fun);
VE_DCL veBool				veItemSetChildAdded(struct VeItem *item, VeItemChildAdded *fun);
VE_DCL void					veItemSetFmt(struct VeItem *item, VeItemValueFmt *fun, void const *ctx);
VE_DCL void					veItemSetGetDescr(struct VeItem *item, VeItemGetDescription *fun);
VE_DCL void					veItemSetGetter(struct VeItem *item, VeItemGetterFun *fun);
VE_DCL void					veItemSetId(struct VeItem *item, char const * id);
VE_DCL void					veItemSetMax(struct VeItem *item, VeVariant *max);
VE_DCL veBool				veItemSetMaxChanged(struct VeItem *item, VeItemValueChanged *fun);
VE_DCL void					veItemSetMin(struct VeItem *item, VeVariant *min);
VE_DCL veBool				veItemSetMinChanged(struct VeItem *item, VeItemValueChanged *fun);
VE_DCL void					veItemSetDefault(struct VeItem *item, VeVariant *min);
VE_DCL veBool				veItemSetDefaultChanged(struct VeItem *item, VeItemValueChanged *fun);
VE_DCL void					veItemSetSetter(struct VeItem *item, VeItemSetterFun *fun, void *setCtx);
VE_DCL void					veItemSetTimeout(struct VeItem *item, un8 seconds);
VE_DCL size_t				veItemUid(struct VeItem *item, char *uid, size_t len);
VE_DCL size_t				veItemRid(struct VeItem *root, struct VeItem *item, char *rid, size_t len);
VE_DCL VeVariant *			veItemValue(struct VeItem *item, VeVariant *var);
VE_DCL size_t				veItemValueFmtString(struct VeItem *item, char *buf, size_t len);
VE_DCL int					veItemValueInt(struct VeItem *root, char const *id);

/* Ve.Bus code uses to bind a item to a specific device. Is this really needed? */
VE_DCL void *				veItemSetCtx(struct VeItem *item);

VE_DCL struct VeDbus *		veItemDbus(struct VeItem *item);
VE_DCL void					veItemSetDbus(struct VeItem *item, struct VeDbus *dbus);

#endif
