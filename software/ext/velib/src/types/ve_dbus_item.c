#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <velib/types/variant.h>
#include <velib/types/ve_dbus_item.h>
#include <velib/types/ve_item.h>
#include <velib/types/ve_str.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_logger.h>

#include "ve_dbus_internal.h"

/**
 * VeItems creates a local tree of items, which can be navigated and
 * values can be get and set etc, see ve_item.c for details. This code
 * binds that tree to the dbus, to allow access to tree from different
 * processes.
 *
 * Currently only support for producers / services exists here.
 * Consumers / clients are in cpp src/qt/ and scripting language can
 * create these objects localy by introspection.
 *
 * @note this is work in progress...
 */

#if 0
#define error(f, ...) logE("dbus", f, ##__VA_ARGS__)
#define info(f, ...) logI("dbus", f, ##__VA_ARGS__)
#else
#define error(...) do {} while(0)
#define info(...) do {} while(0)
#endif

typedef struct {
	DBusMessageIter *itt;
	struct VeItem *root;
} VeDbusDict;

char const *docType = \
	"<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n";

char const *introspectXML = \
	"	<interface name=\"org.freedesktop.DBus.Introspectable\">\n"
	"		<method name=\"Introspect\">\n"
	"			<arg name=\"data\" direction=\"out\" type=\"s\"/>\n"
	"		</method>\n"
	"	</interface>\n"
	"	<interface name=\"org.freedesktop.DBus.Properties\">\n"
	"		<method name=\"Get\">\n"
	"			<arg direction=\"in\" type=\"s\" name=\"interface\"/>\n"
	"			<arg direction=\"in\" type=\"s\" name=\"property\"/>\n"
	"			<arg direction=\"out\" type=\"v\" name=\"value\"/>\n"
	"		</method>\n"
	"	</interface>\n"
	"	<interface name=\"com.victronenergy.BusItem\">\n"
	"		<method name=\"GetDescription\">\n"
	"			<arg direction=\"in\" type=\"s\" name=\"language\"/>\n"
	"			<arg direction=\"in\" type=\"i\" name=\"length\"/>\n"
	"			<arg direction=\"out\" type=\"s\" name=\"descr\"/>\n"
	"		</method>\n"
	"		<method name=\"GetValue\">\n"
	"			<arg direction=\"out\" type=\"v\" name=\"value\"/>\n"
	"		</method>\n"
	"		<method name=\"GetText\">\n"
	"			<arg direction=\"out\" type=\"s\" name=\"value\"/>\n"
	"		</method>\n"
	"		<method name=\"SetValue\">\n"
	"			<arg direction=\"in\" type=\"v\" name=\"value\"/>\n"
	"			<arg direction=\"out\" type=\"i\" name=\"retval\"/>\n"
	"		</method>\n"
	"		<method name=\"GetMin\">\n"
	"			<arg direction=\"out\" type=\"v\" name=\"value\"/>\n"
	"		</method>\n"
	"		<method name=\"GetMax\">\n"
	"			<arg direction=\"out\" type=\"v\" name=\"value\"/>\n"
	"		</method>\n"
	"		<method name=\"SetDefault\">\n"
	"			<arg direction=\"out\" type=\"i\" name=\"retval\"/>\n"
	"		</method>\n"
	"		<method name=\"GetDefault\">\n"
	"			<arg direction=\"out\" type=\"v\" name=\"value\"/>\n"
	"		</method>\n"
	"		<property access=\"readwrite\" type=\"v\" name=\"Value\"/>\n"
	"		<property access=\"readwrite\" type=\"v\" name=\"Min\"/>\n"
	"		<property access=\"readwrite\" type=\"v\" name=\"Max\"/>\n"
	"		<property access=\"read\" type=\"s\" name=\"Text\"/>\n"
	"		<property access=\"read\" type=\"v\" name=\"Default\"/>\n"
	"		<signal name=\"PropertiesChanged\">\n"
	"			<arg name=\"changes\" type=\"a{sv}\" direction=\"out\"/>\n"
	"		</signal>\n"
	"	</interface>\n";

static void veDbusItemRemove(struct VeItem *items);
static veBool (*platformDbusInit)(struct VeDbus *ctx);
static struct VeDbus *defaultVeDbus;
static char const *defaultBusString;

/*
 * Returns the dbus path of item in path
 *
 * Since a subtree might be export the dbus path can differ from the
 * tree path. The path is returned with a leading /.
 */
size_t veDbusItemPath(struct VeItem *item, char *path, size_t len)
{
	struct VeDbus *dbus = veItemDbus(item);
	size_t n;

	path[0] = '/';
	n = veItemRid(dbus->items, item, &path[1], len - 1);
	if (n >= len - 1)
		return n + 1;

	return n + 1;
}

struct VeItem *veDbusItemByUid(struct VeItem *dbusRoot, char const *uid)
{
	if (!uid || uid[0] == 0 || uid[0] != '/')
		return NULL;

	return veItemByUid(dbusRoot, &uid[1]);
}

static DBusMessage* dbusCreateReply(DBusMessage *msg, DBusMessageIter *itt)
{
	DBusMessage* reply;

	/* create a reply from the message */
	if (!(reply = dbus_message_new_method_return(msg))) {
		fprintf(stderr, "dbusCreateReply, out of memory!\n");
		return NULL;
	}

	if (itt)
		dbus_message_iter_init_append(reply, itt);
	return reply;
}

static veBool appendVar(DBusMessageIter *itt, int type, void* val)
{
	DBusMessageIter itt2;
	char str[2] = {type, 0};
	veBool failed;

	if (!dbus_message_iter_open_container(itt, DBUS_TYPE_VARIANT, str, &itt2))
		return veFalse;

	failed = !dbus_message_iter_append_basic(&itt2, type, val);
	failed |= !dbus_message_iter_close_container(itt, &itt2);

	return !failed;
}

veBool veDbusMsgAppendVeVariant(DBusMessageIter *itt, VeVariant *var)
{
	/*
	 * Dbus has no invalid type, "maybes" have been suggested but not in mainline,
	 * So sent an empty array if the value is unknown.
	 */
	if (!veVariantIsValid(var)) {
		DBusMessageIter array;
		DBusMessageIter variant;

		dbus_message_iter_open_container(itt, DBUS_TYPE_VARIANT, "ai", &variant);
		dbus_message_iter_open_container(&variant, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32_AS_STRING, &array);
		dbus_message_iter_close_container(&variant, &array);
		dbus_message_iter_close_container(itt, &variant);

		return veTrue;
	}

	switch (var->type.tp)
	{
	case VE_UN8:
		return appendVar(itt, DBUS_TYPE_BYTE, &var->value.UN8);
	case VE_SN16:
		return appendVar(itt, DBUS_TYPE_INT16, &var->value.SN16);
	case VE_UN16:
		return appendVar(itt, DBUS_TYPE_UINT16, &var->value.UN16);
	case VE_SN32:
		return appendVar(itt, DBUS_TYPE_INT32, &var->value.SN32);
	case VE_UN32:
		return appendVar(itt, DBUS_TYPE_UINT32, &var->value.UN32);
	case VE_FLOAT:
		{
			double tmp = var->value.Float;
			return appendVar(itt, DBUS_TYPE_DOUBLE, &tmp);
		}
	case VE_BIT1:
		{
			dbus_bool_t valid = var->value.UN32;
			return appendVar(itt, DBUS_TYPE_BOOLEAN, &valid);
		}
	case VE_STR:
	case VE_HEAP_STR:
		if (!veVariantIsValid(var)) {
			char *empty = "";
			return appendVar(itt, DBUS_TYPE_STRING, &empty);
		}
		return appendVar(itt, DBUS_TYPE_STRING, &var->value.Ptr);
	case VE_BUF:
	case VE_HEAP:
		{
			DBusMessageIter variant;
			DBusMessageIter arrayItt;

			dbus_message_iter_open_container(itt, DBUS_TYPE_VARIANT, "ay", &variant);
			dbus_message_iter_open_container(&variant, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &arrayItt);
			if (!dbus_message_iter_append_fixed_array(&arrayItt, DBUS_TYPE_BYTE, &var->value.Ptr, var->type.len))
				return veFalse;
			dbus_message_iter_close_container(&variant, &arrayItt);
			if (!dbus_message_iter_close_container(itt, &variant))
				return veFalse;
			return veTrue;
		}
	case VE_FLOAT_ARRAY:
		{
			int i;
			float *array = (float *) var->value.Ptr;

			DBusMessageIter arrayItt;
			DBusMessageIter variant;
			dbus_message_iter_open_container(itt, DBUS_TYPE_VARIANT, "ad", &variant);
			dbus_message_iter_open_container(&variant, DBUS_TYPE_ARRAY, DBUS_TYPE_DOUBLE_AS_STRING, &arrayItt);

			for (i = 0; i < var->type.len; i++) {
				double tmp = (double) array[i];
				dbus_message_iter_append_basic(&arrayItt, DBUS_TYPE_DOUBLE, &tmp);
			}

			dbus_message_iter_close_container(&variant, &arrayItt);
			dbus_message_iter_close_container(itt, &variant);

			return veTrue;
		}
	default:
		/* check if the value can be promoted to a 32 bit number */
		veVariantToN32(var);
		if (var->type.tp == VE_SN32)
			return appendVar(itt, DBUS_TYPE_INT32, &var->value.SN32);
		else if (var->type.tp == VE_UN32)
			return appendVar(itt, DBUS_TYPE_UINT32, &var->value.UN32);
		else
			error("unknown type in variant %d", var->type.tp);

		return veFalse;
	}
}

veBool veDbusMsgReadVeVariant(DBusMessageIter *itt, VeVariant *variant)
{
	DBusMessageIter variantItt;
	int val;
	char *str;

	if (dbus_message_iter_get_arg_type(itt) != DBUS_TYPE_VARIANT)
		return veFalse;

	dbus_message_iter_recurse(itt, &variantItt);
	switch (dbus_message_iter_get_arg_type(&variantItt))
	{
	case DBUS_TYPE_BYTE:
		variant->type.tp = VE_UN8;
		dbus_message_iter_get_basic(&variantItt, &variant->value.UN8);
		return veTrue;
	case DBUS_TYPE_INT16:
		variant->type.tp = VE_SN16;
		dbus_message_iter_get_basic(&variantItt, &variant->value.SN16);
		return veTrue;
	case DBUS_TYPE_UINT16:
		variant->type.tp = VE_UN16;
		dbus_message_iter_get_basic(&variantItt, &variant->value.UN16);
		return veTrue;
	case DBUS_TYPE_INT32:
		variant->type.tp = VE_SN32;
		dbus_message_iter_get_basic(&variantItt, &variant->value.SN32);
		return veTrue;
	case DBUS_TYPE_UINT32:
		variant->type.tp = VE_UN32;
		dbus_message_iter_get_basic(&variantItt, &variant->value.SN32);
		return veTrue;
	case DBUS_TYPE_DOUBLE:
		{
			double tmp;
			variant->type.tp = VE_FLOAT;
			dbus_message_iter_get_basic(&variantItt, &tmp);
			variant->value.Float = (float) tmp;
			return veTrue;
		}
	case DBUS_TYPE_STRING:
		/* hack: use numbers till there is a more generic solution */
		dbus_message_iter_get_basic(&variantItt, &str);
		val = atoi(str);
		veVariantSn32(variant, val);
		break;

#if CFG_VARIANT_HEAP
	case DBUS_TYPE_ARRAY:
		{
			DBusMessageIter arrayItt;

			/* walk through the array */
			dbus_message_iter_recurse(&variantItt, &arrayItt);
			switch (dbus_message_iter_get_arg_type(&arrayItt)) {
			case DBUS_TYPE_BYTE:
				{
					char const *buf;
					int bufLen;

					if (variant->type.tp != VE_HEAP) {
						error("received VE_HEAP but item didn't expect it");
						return veFalse;
					}
					/* note: this obtains a pointer into the message itself */
					dbus_message_iter_get_fixed_array(&arrayItt, &buf, &bufLen);

					if (bufLen > 255) {
						error("array too large %d", bufLen);
						return veFalse;
					}

					/* so make a copy */
					veAssert(variant->value.Ptr == NULL);
					veVariantAlloc(variant, bufLen);
					if (variant->value.Ptr == NULL)
						return veFalse;
					memcpy(variant->value.Ptr, buf, bufLen);
					return veTrue;
				}
			default:
				error("unsupported type in array");
				return veFalse;
			}
		}

		return veFalse;
#endif

	default:
		error("unknown type in variant");
		return veFalse;
	}

	return veTrue;
}

static void msgAppendDictEntry(struct VeItem *item, void *ctx)
{
	DBusMessageIter entry;
	VeDbusDict *dict = (VeDbusDict *)ctx;
	char buf[VE_MAX_UID_SIZE];
	char *id = buf;
	VeVariant var;

	/* Skip item if it has children */
	if (veItemFirstChild(item))
		return;

	/*
	 * Get the item id from the branch that is specified,
	 * this is not necessarliy the root of the tree.
	 */
	if (veItemRid(dict->root, item, buf, sizeof(buf)) >= sizeof(buf))
		return;

	dbus_message_iter_open_container(dict->itt, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &id);

	veItemValue(item, &var);
	veDbusMsgAppendVeVariant(&entry, &var);

	dbus_message_iter_close_container (dict->itt, &entry);
}

/* Append the item tree as a dictionary */
static veBool msgAppendDict(DBusMessageIter *itt, struct VeItem *item)
{
	DBusMessageIter dict;
	DBusMessageIter variant;
	VeDbusDict ctx;

	if (!dbus_message_iter_open_container(itt, DBUS_TYPE_VARIANT, "a{sv}", &variant))
		return veFalse;

	if (!dbus_message_iter_open_container(&variant, DBUS_TYPE_ARRAY, "{sv}", &dict))
		return veFalse;

	ctx.itt = &dict;
	ctx.root = item;
	veItemForeach(item, msgAppendDictEntry, &ctx);

	dbus_message_iter_close_container (&variant, &dict);
	dbus_message_iter_close_container(itt, &variant);

	return veTrue;
}

veBool veDbusSend(DBusConnection *conn, DBusMessage* msg)
{
	/* send the reply */
	if (!dbus_connection_send(conn, msg, NULL)) {
		fprintf(stderr, "dbusSend, out of memory!\n");
		return veFalse;
	}

#if !CFG_DBUS_NON_BLOCKING
	/* block if no dbus mainloop is available */
	dbus_connection_flush(conn);
#endif

	return veTrue;
}

#if CFG_DBUS_NON_BLOCKING
/*
 * Note: hang will cause the dbus to pseudo block. Which basically
 * means the whole thread is devoted to only watch for the reply.
 * It does not reenter a mainloop or serve a message pump at all!
 * If the msg is send to the process itself, it is hence doomed to
 * timeout, since no incoming messages are processed. Likewise if
 * there are two different processes blocking on eachother at the
 * same time, they are doomed to timeout (and stop processing any
 * other messages in the meantime). It should be clear that use
 * of it is strongly discouraged. So why does the parameter exists
 * at all? Well that are some valid usages, e.g. if a tool starts
 * up with the intention to talk with com.victronenergy.example,
 * the nameOwner of the service needs to be looked up / checked
 * if it exists at all. Since that message is send to the dbus
 * process itself and the tool doesn't have much better things to
 * do it might as well block on the call and exit directly if the
 * intended service is unavailable.
 */
veBool veDbusSendWithReply(DBusConnection *conn, DBusMessage* msg,
						   DBusPendingCallNotifyFunction function, void *ctx, veBool hang)
{
	DBusPendingCall *call;

	if (!dbus_connection_send_with_reply(conn, msg, &call, 5000))
		return veFalse;

	dbus_pending_call_set_notify(call, function, ctx, NULL);
	if (hang)
		dbus_pending_call_block(call);
	dbus_pending_call_unref(call);

	return veTrue;
}
#endif

static DBusMessage* handleIntrospect(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	VeStr s;
	struct VeItem *child;

	/* create a reply from the message */
	if ( !(reply = dbus_message_new_method_return(msg)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, "");

	info("introspect %s", item->id);

	veStrNew(&s, 2048, 512);
	veStrSet(&s, docType);
	veStrAdd(&s, "<node>\n");
	veStrAdd(&s, introspectXML);

	/* add child nodes */
	if (item) {
		child = veItemFirstChild(item);

		while (child) {
			veStrAddFormat(&s, "\t<node name=\"%s\"/>\n", veItemId(child));
			child = veItemNextChild(child);
		}
	}

	veStrAdd(&s, "</node>");

	if (s.error) {
		fprintf(stderr, "Out Of Memory!\n");
		goto out;
	}

	/* add the arguments to the reply */
	dbus_message_append_args(reply, DBUS_TYPE_STRING, &s.data, DBUS_TYPE_INVALID);
	veStrFree(&s);

	return reply;

out:
	// free the reply
	dbus_message_unref(reply);
	return NULL;
}

static DBusMessage* handleGetValue(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	DBusMessageIter itt;

	/* create a reply from the message */
	if ( !(reply = dbus_message_new_method_return(msg)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);

	dbus_message_iter_init_append(reply, &itt);

	if (veItemFirstChild(item)) {
		msgAppendDict(&itt, item);
	} else {
		VeVariant var;
		veItemValue(item, &var);
		veDbusMsgAppendVeVariant(&itt, &var);
	}

	return reply;
}

static DBusMessage* handleGetMin(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	DBusMessageIter itt;
	VeVariant min;

	/* create a reply from the message */
	if ( !(reply = dbus_message_new_method_return(msg)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);

	dbus_message_iter_init_append(reply, &itt);
	veItemMin(item, &min);
	veDbusMsgAppendVeVariant(&itt, &min);

	return reply;
}

static DBusMessage* handleGetMax(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	DBusMessageIter itt;
	VeVariant max;

	/* create a reply from the message */
	if ( !(reply = dbus_message_new_method_return(msg)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);

	dbus_message_iter_init_append(reply, &itt);
	veItemMax(item, &max);
	veDbusMsgAppendVeVariant(&itt, &max);

	return reply;
}

static DBusMessage* handleGetDefault(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	DBusMessageIter itt;
	VeVariant defaultValue;

	/* create a reply from the message */
	if ( !(reply = dbus_message_new_method_return(msg)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);

	dbus_message_iter_init_append(reply, &itt);
	veItemDefaultValue(item, &defaultValue);
	veDbusMsgAppendVeVariant(&itt, &defaultValue);

	return reply;
}

static DBusMessage* handleGetText(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	char buf[VE_MAX_UID_SIZE];
	char *text = buf;

	if ( !(reply = dbus_message_new_method_return(msg)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);

	if (veItemValueFmtString(item, buf, sizeof(buf)) >= sizeof(buf))
		goto out;

	dbus_message_append_args(reply, DBUS_TYPE_STRING, &text, DBUS_TYPE_INVALID);
	return reply;

out:
	dbus_message_unref(reply);
	return NULL;
}

static DBusMessage* handleGetDescription(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	char buf[250];
	char *descr = buf;

	if ( !(reply = dbus_message_new_method_return(msg)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);

	if (veItemDescription(item, buf, sizeof(buf)) >= sizeof(buf))
		goto out;

	dbus_message_append_args(reply, DBUS_TYPE_STRING, &descr, DBUS_TYPE_INVALID);

	return reply;

out:
	dbus_message_unref(reply);
	return NULL;
}

static DBusMessage* handleSetValue(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	DBusMessageIter args, itt;
	VeVariant variant;
	sn32 ret = 0;

	/*
	 * Pass in a copy of the type of the variant, so the read routine can
	 * actually check if it is of the valid type / perform a conversion.
	 * Don't pass in the current value though, so it can never be freed
	 * by accident.
	 */
	veItemLocalValue(item, &variant);
	variant.value.Ptr = NULL;

	if (!dbus_message_iter_init(msg, &args) || !veDbusMsgReadVeVariant(&args, &variant))
		return dbus_message_new_error(msg, DBUS_ERROR_INVALID_ARGS, "expected variant");

	if (!veItemSet(item, &variant)) {
		veVariantFree(&variant);
		ret = -1;
	}

	if ( !(reply = dbusCreateReply(msg, &itt)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);

	dbus_message_append_args(reply, DBUS_TYPE_INT32, &ret, DBUS_TYPE_INVALID);
	return reply;
}

static DBusMessage* handleSetDefault(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	DBusMessageIter itt;
	sn32 ret = 0;
	VeVariant defaultValue;

	veItemDefaultValue(item, &defaultValue);
	if (!veVariantIsValid(&defaultValue) || !veItemSet(item, &defaultValue))
		ret = -1;

	if ( !(reply = dbusCreateReply(msg, &itt)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);

	dbus_message_append_args(reply, DBUS_TYPE_INT32, &ret, DBUS_TYPE_INVALID);
	return reply;
}

static DBusMessage* handlePropGet(DBusMessage *msg, struct VeItem *item)
{
	DBusMessage *reply;
	DBusMessageIter args, itt;
	char *interface;
	char *property;
	VeVariant var;

	/* get the interface */
	if (!dbus_message_iter_init(msg, &args) || dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_STRING)
		return dbus_message_new_error(msg, DBUS_ERROR_INVALID_ARGS, "interface");
	dbus_message_iter_get_basic(&args, &interface);

	/* the property being get */
	if (!dbus_message_iter_next(&args) || dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_STRING)
		return dbus_message_new_error(msg, DBUS_ERROR_INVALID_ARGS, "property");
	dbus_message_iter_get_basic(&args, &property);

	info("get %s %s", interface, property);

	if (strcmp(interface, veInterface) != 0)
		return dbus_message_new_error(msg, DBUS_ERROR_UNKNOWN_INTERFACE, NULL);

	if ( !(reply = dbusCreateReply(msg, &itt)) )
		return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);

	if (strcmp(property, "Value") == 0) {
		veItemValue(item, &var);
		veDbusMsgAppendVeVariant(&itt, &var);
	} else if (strcmp(property, "Text") == 0) {
		char buf[VE_MAX_UID_SIZE];
		char *text = buf;

		if (veItemValueFmtString(item, buf, sizeof(buf)) >= sizeof(buf)) {
			dbus_message_unref(reply);
			return dbus_message_new_error(msg, DBUS_ERROR_NO_MEMORY, NULL);
		}
		dbus_message_append_args(reply, DBUS_TYPE_STRING, &text, DBUS_TYPE_INVALID);
	} else if (strcmp(property, "Min") == 0) {
		veItemMin(item, &var);
		veDbusMsgAppendVeVariant(&itt, &var);
	} else if (strcmp(property, "Max") == 0) {
		veItemMax(item, &var);
		veDbusMsgAppendVeVariant(&itt, &var);
	} else if (strcmp(property, "Default") == 0) {
		veItemDefaultValue(item, &var);
		veDbusMsgAppendVeVariant(&itt, &var);
	}

	return reply;
}

typedef enum {
	INTROSPECT,
	PROPERTY_GET,
	GET_VALUE,
	GET_TEXT,
	GET_DESCRIPTION,
	GET_MIN,
	GET_MAX,
	SET_VALUE,
	GET_DEFAULT,
	SET_DEFAULT
} DbusItemMethod;

static DBusHandlerResult handleDbusItemProducerMethods(DBusConnection *connection,
													   DBusMessage *msg, void *data)
{
	DBusMessage* reply = NULL;
	struct VeItem *item;
	char const *uid;
	DbusItemMethod method;
	VeDbus *dbus = (VeDbus *) data;
	VE_UNUSED(connection);

	/* First check is this is a recognized method at all, otherwise let an other handler check it */
	if (dbus_message_is_method_call(msg, "org.freedesktop.DBus.Introspectable", "Introspect"))
		method = INTROSPECT;
	else if (dbus_message_is_method_call(msg, "org.freedesktop.DBus.Properties", "Get"))
		method = PROPERTY_GET;
	else if (dbus_message_is_method_call(msg, veInterface, "GetValue"))
		method = GET_VALUE;
	else if (dbus_message_is_method_call(msg, veInterface, "GetText"))
		method = GET_TEXT;
	else if (dbus_message_is_method_call(msg, veInterface, "GetDescription"))
		method = GET_DESCRIPTION;
	else if (dbus_message_is_method_call(msg, veInterface, "GetMin"))
		method = GET_MIN;
	else if (dbus_message_is_method_call(msg, veInterface, "GetMax"))
		method = GET_MAX;
	else if (dbus_message_is_method_call(msg, veInterface, "SetValue"))
		method = SET_VALUE;
	else if (dbus_message_is_method_call(msg, veInterface, "GetDefault"))
		method = GET_DEFAULT;
	else if (dbus_message_is_method_call(msg, veInterface, "SetDefault"))
		method = SET_DEFAULT;
	else
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	uid = dbus_message_get_path(msg);
	item = veDbusItemByUid(dbus->items, uid);
	if (item == NULL) {
		error("DBUS_ERROR_UNKNOWN_OBJECT");
		reply = dbus_message_new_error(msg, DBUS_ERROR_UNKNOWN_OBJECT, NULL);
	} else {
		switch (method) {
		case INTROSPECT:
			reply = handleIntrospect(msg, item);
			break;
		case PROPERTY_GET:
			reply = handlePropGet(msg, item);
			break;
		case GET_VALUE:
			reply = handleGetValue(msg, item);
			break;
		case GET_TEXT:
			reply = handleGetText(msg, item);
			break;
		case GET_DESCRIPTION:
			reply = handleGetDescription(msg, item);
			break;
		case GET_MIN:
			reply = handleGetMin(msg, item);
			break;
		case GET_MAX:
			reply = handleGetMax(msg, item);
			break;
		case SET_VALUE:
			reply = handleSetValue(msg, item);
			break;
		case GET_DEFAULT:
			reply = handleGetDefault(msg, item);
			break;
		case SET_DEFAULT:
			reply = handleSetDefault(msg, item);
			break;
		}
	}

	if (reply) {
		veDbusSend(dbus->conn, reply);
		dbus_message_unref(reply);
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

/*
 * Callback from the items tree that a item changed, notify
 * the dbus accordingly.
 */
static void onItemChanged(struct VeItem *item)
{
	DBusMessage *msg;
	DBusMessageIter itt, dic, entry;
	char const *valueStr = "Value";
	char const *textStr = "Text";
	char buf[VE_MAX_UID_SIZE];
	struct VeDbus *dbus = veItemDbus(item);
	VeVariant var;
	size_t len;

	if (veDbusItemPath(item, buf, sizeof(buf)) >= sizeof(buf))
		return;

	/* create a signal & check for errors */
	msg = dbus_message_new_signal(buf, veInterface, "PropertiesChanged");

	if (!msg)
		return;

	dbus_message_iter_init_append(msg, &itt);

	/* append a dictionary */
	dbus_message_iter_open_container(&itt, DBUS_TYPE_ARRAY, "{sv}", &dic);

	/* with the changed properties and new values */
	dbus_message_iter_open_container(&dic, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &valueStr);
	veItemValue(item, &var);
	veDbusMsgAppendVeVariant(&entry, &var);
	dbus_message_iter_close_container(&dic, &entry);

	/* add textual version (if any) */
	len = veItemValueFmtString(item, buf, sizeof(buf));
	if (len < sizeof(buf)) {
		VeVariant var;

		dbus_message_iter_open_container(&dic, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
		dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &textStr);
		var.type.tp = VE_STR;
		var.value.Ptr = buf;
		veDbusMsgAppendVeVariant(&entry, &var);
		dbus_message_iter_close_container(&dic, &entry);
	}

	/* end of dictionary */
	dbus_message_iter_close_container(&itt, &dic);

	veDbusSend(dbus->conn, msg);

	/* free the message */
	dbus_message_unref(msg);
}

/** Callback from the items tree that the span changes */
static void onMinMaxDefaultChanged(struct VeItem *item)
{
	DBusMessage *msg;
	DBusMessageIter itt, dic, entry;
	char const *minStr = "Min";
	char const *maxStr = "Max";
	char const *defStr = "Default";
	char buf[VE_MAX_UID_SIZE];
	struct VeDbus *dbus = veItemDbus(item);
	VeVariant var;

	if (veDbusItemPath(item, buf, sizeof(buf)) >= sizeof(buf))
		return;

	/* create a signal & check for errors */
	msg = dbus_message_new_signal(buf, veInterface, "PropertiesChanged");

	if (!msg)
		return;

	dbus_message_iter_init_append(msg, &itt);

	/* append a dictionary */
	dbus_message_iter_open_container(&itt, DBUS_TYPE_ARRAY, "{sv}", &dic);

	/* add min property */
	veItemMin(item, &var);
	if (veVariantIsValid(&var)) {
		dbus_message_iter_open_container(&dic, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
		dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &minStr);
		veDbusMsgAppendVeVariant(&entry, &var);
		dbus_message_iter_close_container(&dic, &entry);
	}

	/* add max property */
	veItemMax(item, &var);
	if (veVariantIsValid(&var)) {
		dbus_message_iter_open_container(&dic, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
		dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &maxStr);
		veDbusMsgAppendVeVariant(&entry, &var);
		dbus_message_iter_close_container(&dic, &entry);
	}

	/* add default property */
	veItemDefaultValue(item, &var);
	if (veVariantIsValid(&var)) {
		dbus_message_iter_open_container(&dic, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
		dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &defStr);
		veDbusMsgAppendVeVariant(&entry, &var);
		dbus_message_iter_close_container(&dic, &entry);
	}

	/* end of dictionary */
	dbus_message_iter_close_container(&itt, &dic);

	veDbusSend(dbus->conn, msg);

	/* free the message */
	dbus_message_unref(msg);
}

static void setItemProperties(struct VeItem *item, void *ctx);

/* set properties if parent has dbus enabled */
static void onChildAdded(struct VeItem *item)
{
	struct VeItem *parent = veItemParent(item);
	struct VeDbus *dbus = veItemDbus(parent);

	if (dbus)
		setItemProperties(item, dbus);
}

/* register the onchange callback to forward changes to the dbus */
static void setItemProperties(struct VeItem *item, void *ctx)
{
	veAssert(veItemDbus(item) == NULL);

	veItemSetChanged(item, onItemChanged);
	veItemSetMinChanged(item, onMinMaxDefaultChanged);
	veItemSetMaxChanged(item, onMinMaxDefaultChanged);
	veItemSetDefaultChanged(item, onMinMaxDefaultChanged);
	veItemSetChildAdded(item, onChildAdded);
	veItemSetAboutToRemoved(item, veDbusItemRemove);
	veItemSetDbus(item, ctx);
}

static void removeItemProperties(struct VeItem *item, void *ctx)
{
	VE_UNUSED(ctx);

	veItemSetChanged(item, NULL);
	veItemSetMinChanged(item, NULL);
	veItemSetMaxChanged(item, NULL);
	veItemSetDefaultChanged(item, NULL);
	veItemSetChildAdded(item, NULL);
	veItemSetAboutToRemoved(item, NULL);
	veItemSetDbus(item, NULL);
}

/**
 * Exports the item tree to the dbus
 *
 * @param conn	the dbus connection
 * @param items	the item tree to export
 *
 * @note only one tree can be exported
 */
void veDbusItemInit(VeDbus *dbus, struct VeItem *items)
{
	veAssert(dbus->items == NULL);

	dbus->items = items;
	veItemForeach(items, setItemProperties, dbus);
}

static void veDbusItemRemove(struct VeItem *items)
{
	struct VeDbus *dbus = veItemDbus(items);

	if (dbus->items == items)
		dbus->items = NULL;
	veItemForeach(items, removeItemProperties, NULL);
}

VeDbus *veDbusConnectString(char const *address)
{
	DBusError err;
	struct VeDbus *dbus = calloc(1, sizeof(VeDbus));
	veBool knownBus = veTrue;

	if (!dbus)
		return NULL;

	dbus_error_init(&err);

	/* connect to the bus and check for errors */
	if (strcmp(address, "session") == 0) {
		dbus->conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
	} else if (strcmp(address, "system") == 0) {
		dbus->conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
	} else {
		knownBus = veFalse;
		dbus->conn = dbus_connection_open_private(address, &err);
	}

	if (!dbus->conn) {
		dbus_error_free(&err);
		goto connect_error;
	}

	/* non standard busses need some additional setup */
	if (!knownBus) {
		if (!dbus_bus_register(dbus->conn, &err)) {
			dbus_error_free(&err);
			dbus_connection_close(dbus->conn);
			dbus_connection_unref(dbus->conn);
			goto connect_error;
		}

		/* By default we're bound to the lifecycle of the message bus. */
		dbus_connection_set_exit_on_disconnect(dbus->conn, TRUE);
	}

	if (platformDbusInit) {
		if (!platformDbusInit(dbus)) {
			fprintf(stderr, "dbus platform init failed");
			goto connect_error;
		}
	} else {
		fprintf(stderr, "a platform backend is missing, expect problems!\n");
	}

	if (!dbus_connection_add_filter(dbus->conn, handleDbusItemProducerMethods, dbus, NULL)) {
		error("dbus_connection_add_filter() failed");
		goto connect_error;
	}

	return dbus;

connect_error:
	free(dbus);
	return NULL;
}

void veDbusSetDefaultConnectString(char const *address)
{
	defaultBusString = address;
}

char const *veDbusGetDefaultConnectString(void)
{
	/* non X user don't have a session bus, so use the system bus there by default */
	if (defaultBusString == NULL)
		defaultBusString = (getenv("DBUS_SESSION_BUS_ADDRESS") ? "session" : "system");

	return defaultBusString;
}

VeDbus *veDbusGetDefaultBus(void)
{
	if (!defaultVeDbus)
		defaultVeDbus = veDbusConnectString(veDbusGetDefaultConnectString());
	return defaultVeDbus;
}

VeDbus* veDbusConnect(DBusBusType dbusType)
{
	switch (dbusType)
	{
	case DBUS_BUS_SESSION:
		return veDbusConnectString("session");
	case DBUS_BUS_SYSTEM:
		return veDbusConnectString("system");
	default:
		return NULL;
	}
}

veBool veDbusChangeName(VeDbus *dbus, char const *name)
{
	DBusError err;

	free(dbus->serviceName);
	dbus->serviceName = NULL;
	dbus_error_init(&err);

	dbus_bus_request_name(dbus->conn, name, DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
	if (dbus_error_is_set(&err)) {
		fprintf(stderr, "Request Name Error (%s)\n", err.message);
		dbus_error_free(&err);
		return veFalse;
	}

	/* remember name */
	dbus->serviceName = strdup(name);
	return veTrue;
}

static void freeDbusConnection(VeDbus *dbus)
{
	free(dbus->serviceName);
	free(dbus);
}

/** remove the connection from the dbus, any items will be removed */
void veDbusDisconnect(VeDbus *dbus)
{
	DBusError err;

	dbus_error_init(&err);
	dbus_bus_release_name(dbus->conn, dbus->serviceName, &err);
	if (dbus_error_is_set(&err))
	{
		fprintf(stderr, "Release Name Error (%s)\n", err.message);
		dbus_error_free(&err);
	}

	if (dbus->items)
		veDbusItemRemove(dbus->items);

	dbus_connection_close(dbus->conn);
	dbus_connection_unref(dbus->conn);

	freeDbusConnection(dbus);
}

char const *veDbusServiceName(VeDbus *dbus)
{
	return dbus->serviceName;
}

/* the blocking version of dbus needs updates */
void veDbusItemUpdate(VeDbus *dbus)
{
	DBusMessage *msg;

	if (platformDbusInit != NULL)
		return;

	/* loop listening for signals being emmitted */
	for (;;)
	{
		/* non blocking read of the next available message */
		dbus_connection_read_write(dbus->conn, 0);
		if (!(msg = dbus_connection_pop_message(dbus->conn)))
				return;

		// NOTE: only handles the producer side
		handleDbusItemProducerMethods(dbus->conn, msg, dbus);

		/* free the message */
		dbus_message_unref(msg);
	}
}

/* The part below is optional, it makes the dbus calls non-blocking with libevent. */
#if CFG_WITH_LIBEVENT

struct event_base *libeventBase;

/* libevent support */
static void dispatch(int fd, short ev, void *x)
{
	VeDbus *dbus = x;
	VE_UNUSED(fd);
	VE_UNUSED(ev);

	info("dispatching");

	while (dbus_connection_get_dispatch_status(dbus->conn) == DBUS_DISPATCH_DATA_REMAINS)
		dbus_connection_dispatch(dbus->conn);
}

static void handleWatch(int fd, short events, void *x)
{
	struct DBusWatch *watch = x;
	unsigned int flags = 0;
	VE_UNUSED(fd);

	if (events & EV_READ)
		flags |= DBUS_WATCH_READABLE;
	if (events & EV_WRITE)
		flags |= DBUS_WATCH_WRITABLE;

	info("got dbus watch event fd=%d watch=%p ev=%d", fd, watch, events);
	if (dbus_watch_handle(watch, flags) == FALSE)
		error("handleWatch() failed");
}

static dbus_bool_t addwatch(DBusWatch *watch, void *data)
{
	struct event *event;
	int fd;
	unsigned int flags;
	short cond;
	VE_UNUSED(data);

	if (!dbus_watch_get_enabled(watch))
		return TRUE;

	fd = dbus_watch_get_unix_fd(watch);
	flags = dbus_watch_get_flags(watch);
	cond = EV_PERSIST;
	if (flags & DBUS_WATCH_READABLE)
		cond |= EV_READ;
	if (flags & DBUS_WATCH_WRITABLE)
		cond |= EV_WRITE;

	event = event_new(libeventBase, fd, cond, handleWatch, watch);
	if (!event)
		return FALSE;

	dbus_watch_set_data(watch, event, NULL);
	if (event_add(event, NULL) < 0)
		return FALSE;

	info("added dbus watch fd=%d watch=%p cond=%d", fd, watch, cond);

	return TRUE;
}

static void removeWatch(DBusWatch *w, void *data)
{
	struct event *event = dbus_watch_get_data(w);
	VE_UNUSED(data);

	if (event)
		event_free(event);

	dbus_watch_set_data(w, NULL, NULL);

	info("removed dbus watch watch=%p", w);
}

static void toggleWatch(DBusWatch *w, void *data)
{
	info("toggling dbus watch watch=%p", w);

	if (dbus_watch_get_enabled(w))
		addwatch(w, data);
	else
		removeWatch(w, data);
}

/* Tiemout handling */
static void handleTimeout(int fd, short ev, void *dbusTimeout)
{
	DBusTimeout *t = dbusTimeout;
	VE_UNUSED(fd);
	VE_UNUSED(ev);

	info("got dbus handle timeout event %p", t);

	dbus_timeout_handle(t);
}

static dbus_bool_t addTimeout(DBusTimeout *t, void *data)
{
	struct event *event;
	int ms;
	struct timeval tv;
	VE_UNUSED(data);

	if (!dbus_timeout_get_enabled(t))
		return TRUE;

	info("adding timeout %p", t);

	event = event_new(libeventBase, -1, EV_TIMEOUT|EV_PERSIST,
									handleTimeout, t);
	if (!event) {
		info("failed to allocate new event for timeout");
		return FALSE;
	}

	ms = dbus_timeout_get_interval(t);
	tv.tv_sec = ms / 1000;
	tv.tv_usec = (ms % 1000) * 1000;

	event_add(event, &tv);
	dbus_timeout_set_data(t, event, NULL);

	return TRUE;
}

static void removeTimeout(DBusTimeout *t, void *data)
{
	struct event *event = dbus_timeout_get_data(t);
	VE_UNUSED(data);

	info("removing timeout %p", t);
	event_free(event);
	dbus_timeout_set_data(t, NULL, NULL);
}

static void toggleTimeout(DBusTimeout *t, void *data)
{
	info("toggling timeout %p", t);

	if (dbus_timeout_get_enabled(t))
		addTimeout(t, data);
	else
		removeTimeout(t, data);
}

static void wakeupEvent(void *ctx)
{
	struct VeDbus *dbus = (VeDbus *) ctx;
	static struct timeval tv;

	event_add(&dbus->dispatchEvent, &tv);
}

static veBool dbusInitLibEvent(struct VeDbus *dbus)
{
	if (event_assign(&dbus->dispatchEvent, libeventBase, -1, EV_TIMEOUT, dispatch, dbus) < 0) {
		error("event_assign(&ctx->dispatchEvent,..) failed");
		return veFalse;
	}

	if (!dbus_connection_set_watch_functions(dbus->conn, addwatch, removeWatch,
											 toggleWatch, dbus, NULL)) {
		error("dbus_connection_set_watch_functions() failed");
		return veFalse;
	}

	if (!dbus_connection_set_timeout_functions(dbus->conn, addTimeout,
						removeTimeout, toggleTimeout, dbus, NULL)) {
		error("dbus_connection_set_timeout_functions() failed");
		return veFalse;
	}

	dbus_connection_set_wakeup_main_function(dbus->conn, wakeupEvent, dbus, NULL);

	return veTrue;
}

void veDbusUseLibEvent(struct event_base *base)
{
	libeventBase = base;
	platformDbusInit = dbusInitLibEvent;
}

#endif
