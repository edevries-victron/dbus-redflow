#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <velib/base/ve_string.h>
#include <velib/types/ve_str.h>
#include <velib/types/ve_item.h>
#include <velib/types/ve_item_def.h>
#include <velib/utils/ve_assert.h>

/*
 * VeItem is a tree of VeVariants indexed by unique strings (uid).
 * Made up of all the id's seperated by a /, so e.g.
 *
 *	/
 *	/multi
 *  /multi/dc
 *	/multi/dc/voltage
 *  /multi/dc/current
 *
 * with some additional features. At the moment:
 *
 *	- timeouts to invalidate the received value
 *	- callback when the value changes (only one for now).
 *	- formatter callback to create a string representation.
 *	- option to register the values on dbus, see ve_dbus_item.
 *
 * All childs know their parents and their older / younger "brother".
 * Parents only know the oldest and youngest child, but they in their
 * turn know the ones nearest to them. An attempt to illustrate this:
 *
 * parent
 *  parent->oldest                                parent->youngest
 *  |  child1-^parent       child2-^parent        child3-^parent
 *  |  child1->younger      child2->younger       child3->younger=0
 *  |  child1-<older=0      child2<-older         child3<-older
 *
 * note: heap is used.
 */

#define SEP "/"
#define SEP_C	(SEP[0])

/**
 * Set the childs id to id and adds it to parent when not NULL.
 * @note this is a static version, don't try to free it!
 */
VeItem *veItemInit(VeItem *child, char *id, VeItem *parent)
{
	child->id = id;

	if (parent)
		veItemAddChild(parent, child);

	return child;
}

/** Duplicates the childs id to id and adds it to parent when not NULL */
VeItem *veItemAlloc(VeItem *parent, char const *id)
{
	VeItem* child;

	if ( (child = calloc(1, sizeof(VeItem))) == NULL )
		return NULL;
	child->itemAllocated = 1;

	child->id = strdup(id);
	if (!child->id) {
		free(child);
		return NULL;
	}
	child->idAllocated = 1;

	if (parent)
		veItemAddChild(parent, child);

	return child;
}

/**
 * Add child to a parent.
 *
 * If the parent has a childAddedFunction registered it will be
 * called after adding the child. The child is referenced and needs
 * be valid till removed from the tree.
 */
void veItemAddChild(VeItem *parent, VeItem *child)
{
	veAssert(child->parent == NULL);

	child->parent = parent;

	if (!parent)
		return;

	if (parent->oldest == NULL) {
		/* The first child is the oldest child */
		parent->oldest = child;
		child->younger = NULL;
		child->older = NULL;
	} else {
		parent->youngest->younger = child;
		child->older = parent->youngest;
	}

	parent->youngest = child;

	if (parent->childAddedFun)
		parent->childAddedFun(child);
}

/** @note, only direct siblings only by id */
VeItem *veItemGetChild(VeItem *item, char const *id)
{
	VeItem *child = item->youngest;
	while (child) {
		if (strcmp(child->id, id) == 0)
			return child;
		child = child->older;
	}
	return NULL;
}

/**
 * Get or create a direct sibling of item.
 * The return values is malloced.
 */
VeItem *veItemGetOrCreateChild(VeItem *item, char const *id)
{
	VeItem *child = veItemGetChild(item, id);
	if (child)
		return child;
	return veItemAlloc(item, id);
}

/**
 * Mallocs a descendent and its parents.
 *
 * @param item The item to add the path to.
 * @param uid The path to be created
 * @retval the newly added item
 */
VeItem *veItemGetOrCreateUid(VeItem *item, char const *uid)
{
	char *p;
	char *q;
	char *idCopy;
	char *end;

	if (!(idCopy = strdup(uid)))
		return NULL;
	end = strchr(idCopy, 0);
	q = idCopy;

	while (item)
	{
		p = strchr(q, SEP_C);
		if (!p)
			p = end;
		*p = 0;

		item = veItemGetOrCreateChild(item, q);

		if (p == end)
			break;

		q = p + 1;
	}

	free(idCopy);
	return item;
}

/**
 * Register an item in the tree.
 *
 * @note Since the item is referenced and not copied it should remain valid.
 * This is useful since you "well know items" in a project, so don't need
 * to be searched for, they are just there.
 *
 * @note for convenience missing parents are malloced.
 */
VeItem *veItemAddChildByUid(VeItem *item, char const *uid, VeItem *child)
{
	char *p;
	char *q;
	char *idCopy;
	char *end;

	if (!(idCopy = strdup(uid)))
		return NULL;
	end = strchr(idCopy, 0);
	q = idCopy;

	while (item)
	{
		p = strchr(q, SEP_C);
		if (!p)
			p = end;
		*p = 0;

		if (p != end) {
			// Check / add the stubs in between
			item = veItemGetOrCreateChild(item, q);
		} else {
			if (!child->id) {
				child->id = strdup(q);
				if (!child->id) {
					item = NULL;
					goto out;
				}
				child->idAllocated = 1;
			}
			veItemAddChild(item, child);
		}

		if (p == end)
			break;

		q = p + 1;
	}

out:
	free(idCopy);
	return item;
}

static void veItemPopChild(VeItem *item)
{
	/* Invalidate / free the value in the item */
	veItemInvalidate(item);

	/* Inform the consumers the item is being removed */
	if (item->beforeRemoveFun)
		item->beforeRemoveFun(item);

	/* Inform the family their relative is being killed */
	if (item->older)
		item->older->younger = item->younger;

	if (item->younger)
		item->younger->older = item->older;

	if (item->parent) {
		if (item->parent->oldest == item)
			item->parent->oldest = item->younger;

		if (item->parent->youngest == item)
			item->parent->youngest = item->older;
	}

	item->older = NULL;
	item->younger = NULL;
	item->parent = NULL;
}

static void veItemDelete(VeItem *item, void *ctx)
{
	VE_UNUSED(ctx);

	veItemPopChild(item);

	if (item->idAllocated) {
		free(item->id);
		item->id = NULL;
	}

	if (item->itemAllocated)
		free(item);
}

void veItemDeleteBranch(VeItem *item)
{
	veItemForeachChildsFirst(item, veItemDelete, NULL);
}

/**
 * returns the relative id of the item till the specified parent.
 * e.g. /some/path/to/a with root set to /some will return
 * path/to/a, without a leading /.
 */
size_t veItemRid(VeItem *root, VeItem *item, char *rid, size_t len)
{
	size_t n = 0;

	if (item->parent && item->parent != root) {
		/* add id of the parent(s) */
		n = veItemRid(root, item->parent, rid, len);

		/* add seperator if it fits */
		if (n < len) {
			if (n > 0) {
				len -= n;
				rid[n] = SEP_C;
				n++; len--;
			}
		} else {
			return n + 1;
		}
	}

	/* note: root might not have an id */
	if (item->parent)
		n += ve_snprintf(&rid[n], len, "%s", item->id);
	else
		rid[0] = '\0';

	return n;
}

/** returns the unique id of the item */
size_t veItemUid(VeItem *item, char *uid, size_t len)
{
	/* create the root / */
	ve_snprintf(uid, len, "%s", SEP);
	return veItemRid(NULL, item, &uid[1], len - 1) + 1;
}

VeItem *veItemRoot(VeItem *item)
{
	while (veItemParent(item))
		item = veItemParent(item);

	return item;
}

char const *veItemId(VeItem *item)
{
	return item->id;
}

void veItemSetId(VeItem *item, char const * id)
{
	if (item->idAllocated)
		free(item->id);
	item->id = strdup(id);
	item->idAllocated = veTrue;
}


VeItem *veItemByUid(VeItem *item, char const *uid)
{
	char *p;
	char *q;
	char *idCopy;
	char *end;

	if (!(idCopy = strdup(uid)))
		return NULL;
	q = idCopy;
	end = strchr(idCopy, 0);

	if (*q == SEP_C)
		item = veItemRoot(item);

	while (item)
	{
		p = strchr(q, SEP_C);
		if (!p)
			p = end;
		*p = 0;

		if (strcmp(q, "") != 0)
			item = veItemGetChild(item, q);

		if (p == end)
			break;

		q = p + 1;
	}

	free(idCopy);
	return item;
}

/*
 * Sets the VeItem
 * The valid time is reset.
 * The changedFun is NOT called if the variant is changed.
 * note: This function should only called by the VeItem owner.
 */
void veItemLocalSet(VeItem *item, VeVariant *variant)
{
	veVariantFree(&item->variant);
	item->variant = *variant;

	item->validTime = item->validTimeReset;
}

/*
 * Sets the VeItem.
 * The valid time is reset.
 * The changedFun is called if the variant is changed.
 * note: This function should only called by the VeItem owner.
 */
veBool veItemOwnerSet(VeItem *item, VeVariant *variant)
{
	veBool changed;

	/* "normal" values are directly updated */
	changed = !veVariantIsEqual(&item->variant, variant);
	veVariantFree(&item->variant);
	item->variant = *variant;

	item->validTime = item->validTimeReset;

	if (changed && item->changedFun)
		item->changedFun(item);

	return veTrue;
}

/*
 * Sets the VeItem.
 * The callback setFun is called, which can send a message
 * to the corresponding device.
 * note: This function should be called by the Gui or Dbus.
 */
veBool veItemSet(VeItem *item, VeVariant *variant)
{
	if (item->setFun) {
		/* a callback function is used to update the item */
		if (!item->setFun(item, item->setCtx, variant))
			return veFalse;
		return veTrue;
	}

	return veItemOwnerSet(item, variant);
}

VeVariant * veItemValue(VeItem *item, VeVariant *var)
{
	if (item->getFun)
		item->getFun(item);

	*var = item->variant;
	return var;
}

VeVariant *veItemLocalValue(VeItem *item, VeVariant *var)
{
	*var = item->variant;
	return var;
}

size_t veItemValueFmtString(VeItem *item, char *buf, size_t len)
{
	VeVariant var;

	if (!item->toString) {
		buf[0] = '\0';
		return 0;
	}

	veItemValue(item, &var);
	return item->toString(&var, item->toStringCtx, buf, len);
}

int veItemValueInt(VeItem *root, char const *id)
{
	VeVariant var;
	VeItem* item = veItemByUid(root, id);
	if (item == NULL)
		return -1;

	veItemValue(item, &var);
	veVariantToN32(&var);

	return var.value.UN32;
}

/*
 * Registers a function to be change the value in a pretty string.
 * e.g. with units / description of a enum etc. Only one callback
 * is allowed.
 */
void veItemSetFmt(VeItem *item, VeItemValueFmt *fun, void const *ctx)
{
	item->toString = fun;
	item->toStringCtx = ctx;
}

/*
 * Registers a function to handle the update of an item instead of directly
 * setting it. This allows for boundary checks and forwarding the new value
 * etc. Only one callback is allowed.
 *
 * NOTE: in case the new values is dynamically allocated and accepted by the
 * callback (returns veTrue), the callback is responsible for freeing the data!
 * When the callback assigns the value to the item with veItemOwnerSet
 * the old data will be freed automatically. So a callback only has to deal
 * with freeing the value when it accepts it but never assigns it.
 *
 * Furthermore note that dynamic allocated data is only accepted if the
 * type of the variant explicitly states it can handle it, VE_HEAP e.g.
 */
void veItemSetSetter(VeItem *item, VeItemSetterFun *fun, void *setCtx)
{
	item->setFun = fun;
	item->setCtx = setCtx;
}

void veItemSetGetter(VeItem *item, VeItemGetterFun *fun)
{
	item->getFun = fun;
}

void veItemSetGetDescr(VeItem *item, VeItemGetDescription *fun)
{
	item->getDescr = fun;
}

size_t veItemDescription(VeItem *item, char *buf, size_t len)
{
	if (item->getDescr == NULL)
		return ve_snprintf(buf, len, "no description");

	return item->getDescr(item, buf, len);
}

/*
 * Register a on change event handler. There could be more of these, but
 * current implementation only allow a single function.
 */
veBool veItemSetChanged(VeItem *item, VeItemValueChanged *fun)
{
	if (fun && item->changedFun)
		return veFalse;
	item->changedFun = fun;
	return veTrue;
}

veBool veItemSetMinChanged(VeItem *item, VeItemValueChanged *fun)
{
	if (fun && item->minChangedFun)
		return veFalse;
	item->minChangedFun = fun;
	return veTrue;
}

veBool veItemSetMaxChanged(VeItem *item, VeItemValueChanged *fun)
{
	if (fun && item->maxChangedFun)
		return veFalse;
	item->maxChangedFun = fun;
	return veTrue;
}

veBool veItemSetDefaultChanged(VeItem *item, VeItemValueChanged *fun)
{
	if (fun && item->defaultChangedFun)
		return veFalse;
	item->defaultChangedFun = fun;
	return veTrue;
}

veBool veItemSetChildAdded(VeItem *item, VeItemChildAdded *fun)
{
	if (fun && item->childAddedFun)
		return veFalse;
	item->childAddedFun = fun;
	return veTrue;
}

veBool veItemSetAboutToRemoved(VeItem *item, VeItemAboutToBeRemoved *fun)
{
	if (fun && item->beforeRemoveFun)
		return veFalse;
	item->beforeRemoveFun = fun;

	return veTrue;
}

/* @note: this does invoke the onchange callback but not the set routine */
void veItemInvalidate(VeItem *item)
{
	VeVariant invalid;

	/* copy content and not just type as some types do not invalidate on value */
	invalid = item->variant;
	veVariantInvalidate(&invalid);
	veItemOwnerSet(item, &invalid);
}

veBool veItemIsValid(VeItem *item)
{
	if (item->validTimeReset && !item->validTime)
		return veFalse;

	return veVariantIsValid(&item->variant);
}

static void itemSecTick(VeItem *item, void *ctx)
{
	VE_UNUSED(ctx);

	if (item->validTime && --item->validTime == 0)
		veItemInvalidate(item);
}

void veItemSetTimeout(VeItem *item, un8 seconds)
{
	item->validTimeReset = seconds;
}

void veItemSetMin(VeItem *item, VeVariant *min)
{
	veBool changed = !veVariantIsEqual(&item->min, min);

	item->min = *min;
	if (changed && item->minChangedFun)
		item->minChangedFun(item);
}

void veItemSetMax(VeItem *item, VeVariant *max)
{
	veBool changed = !veVariantIsEqual(&item->max, max);

	item->max = *max;
	if (changed && item->maxChangedFun)
		item->maxChangedFun(item);
}

void veItemSetDefault(VeItem *item, VeVariant *def)
{
	veBool changed = !veVariantIsEqual(&item->defaultValue, def);

	item->defaultValue = *def;
	if (changed && item->defaultChangedFun)
		item->defaultChangedFun(item);
}

VeItem *veItemFirstChild(VeItem *parent)
{
	return parent->oldest;
}

/* note, pass the child! */
VeItem *veItemNextChild(VeItem *child)
{
	return child->younger;
}

/* Invokes fun for item and all it's children */
void veItemForeach(VeItem *item, VeItemForeachFun *fun, void *ctx)
{
	VeItem *start = item;

	while (item)
	{
		fun(item, ctx);

		/* vertical */
		if (item->youngest) {
			item = item->youngest;
		/* horizontal */
		} else if (item->older) {
			/*
			 * If there are no children, the stop condition would
			 * be missed when going "up". So explicitly check here
			 * to not loop through the "brothers" of the rootItem.
			 */
			if (item == start)
				return;
			item = item->older;
		} else {
			/* go up again (or NULL) */
			while (item) {
				item = item->parent;

				/* stop when being back at the start item */
				if (item == start)
					return;

				if (item && item->older) {
					item = item->older;
					break;
				}
			}
		}
	}
}

/*
 * Invokes fun for item and all it's children, but starts with the children.
 * @warning this function recurses!
 */
void veItemForeachChildsFirst(VeItem *item, VeItemForeachFun *fun, void *ctx)
{
	VeItem *child = item->oldest;

	while (child) {
		VeItem *next = child->younger;
		veItemForeachChildsFirst(child, fun, ctx);
		child = next;
	}
	fun(item, ctx);
}

VeItem *veItemParent(VeItem *item)
{
	return item->parent;
}

VeItemCtx *veItemCtx(struct VeItem *item)
{
	return &item->ctx;
}

void *veItemSetCtx(struct VeItem *item)
{
	return &item->setCtx;
}

struct VeDbus *veItemDbus(struct VeItem *item)
{
	return item->dbus;
}

void veItemSetDbus(struct VeItem *item, struct VeDbus *dbus)
{
	item->dbus = dbus;
}

void veItemMax(struct VeItem *item, VeVariant *max)
{
	*max = item->max;
}

void veItemMin(struct VeItem *item, VeVariant *min)
{
	*min = item->min;
}

void veItemDefaultValue(struct VeItem *item, VeVariant *defaultValue)
{
	*defaultValue = item->defaultValue;
}

void veItemTick(VeItem *root)
{
	static un8 sec;

	if (++sec >= 20) {
		sec = 0;
		veItemForeach(root, itemSecTick, NULL);
	}
}

static void print(VeItem *item, void *ctx)
{
	char buf[100];
	VE_UNUSED(ctx);

	veItemUid(item, buf, sizeof(buf));
	printf("%s\n", buf);
}

void veItemDump(VeItem *item)
{
	veItemForeach(item, print, NULL);
}
