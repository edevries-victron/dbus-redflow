#include <velib/base/ve_string.h>
#include <velib/types/variant.h>
#include <velib/types/ve_item.h>
#include <velib/types/ve_item_def.h>
#include <velib/types/ve_values.h>

static struct VeItem root;

struct VeItem *veValueTree(void)
{
	return &root;
}

/**
 * Get the value as an integer.
 *
 * @param uid the identifer to get the value from.
 */
int veValueGetInt(char const *uid)
{
	return veItemValueInt(&root, uid);
}

/**
 * Set an item to an integer value.
 *
 * @param uid	uid of the identifer to set.
 * @param value the value to set
 */
int veValueSetInt(char const *uid, int value)
{
	VeVariant var;
	struct VeItem *item = veItemByUid(&root, uid);
	if (item == NULL)
		return -1;

	veVariantSn32(&var, value);
	veItemSet(item, &var);

	return 0;
}

/**
 * Set an item to an integer value.
 *
 * @param uid	uid of the identifer to set.
 * @param value the value to set
 */
int veValueSetFloat(char const *uid, float value)
{
	VeVariant var;
	struct VeItem *item = veItemByUid(&root, uid);
	if (item == NULL)
		return -1;

	veVariantFloat(&var, value);
	veItemSet(item, &var);

	return 0;
}

/**
 * Obtain the string representation of an item
 *
 * @param uid	the unique id of an item (absolute)
 * @param buf	the buffer to store the string in
 * @param length the size of the buffer
 *
 * @retval the number of characters placed in the buffer.
 */
int veValueGetText(char const *uid, char *buf, int length)
{
	struct VeItem *item = veItemByUid(&root, uid);
	if (item == NULL)
		return ve_snprintf(buf, length, "unknown id");

	return veItemValueFmtString(item, buf, length);
}

