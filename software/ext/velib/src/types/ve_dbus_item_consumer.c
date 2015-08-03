#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <velib/platform/console.h>
#include <velib/types/variant.h>
#include <velib/types/ve_dbus_item.h>
#include <velib/types/ve_item.h>
#include <velib/types/ve_str.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_logger.h>

#include "ve_dbus_internal.h"

#if 0
#define error(f, ...) logE("dbus-cons", f, ##__VA_ARGS__)
#define info(f, ...) logI("dbus-cons", f, ##__VA_ARGS__)
#else
#define error(...) do {} while(0)
#define info(...) do {} while(0)
#endif

static void removeItemProperties(struct VeItem *item, void *ctx);
static void setItemProperties(struct VeItem *item, void *ctx);

static VeDbus const *listeningDbus;

void veDbusSetListeningDbus(struct VeDbus *dbus)
{
	if (listeningDbus)
		return;

	listeningDbus = dbus;
}

struct VeDbus const *veDbusGetListeningDbus(void)
{
	return listeningDbus;
}

/* Get the dbus connection which owns the name */
static void gotNameOwner(DBusPendingCall *pending, void *ctx)
{
	DBusError error;
	DBusMessage *reply;
	char *owner;
	VeRemoteService *dbus = (VeRemoteService *) ctx;
	char const *errorString;

	reply = dbus_pending_call_steal_reply(pending);
	if (reply == NULL)
		return;

	errorString = dbus_message_get_error_name(reply);
	if (errorString) {
		if (strcmp(errorString, DBUS_ERROR_NAME_HAS_NO_OWNER) == 0) {
			info("%s", errorString);
			free(dbus->uniqueName);
			dbus->uniqueName = NULL;
		} else {
			error("%s", errorString);
		}

		dbus_message_unref(reply);
		return;
	}

	dbus_error_init(&error);
	if (!dbus_message_get_args(reply, &error, DBUS_TYPE_STRING, &owner, DBUS_TYPE_INVALID)) {
		dbus_message_unref(reply);
		dbus_error_free(&error);
		return;
	}

	free(dbus->uniqueName);
	dbus->uniqueName = strdup(owner);
	info("gotNameOwner %s = %s", dbus->serviceName, dbus->uniqueName);

	dbus_message_unref(reply);
}

/* Checks if the name has an owner */
static veBool getNameOwner(VeRemoteService *remoteService, veBool block)
{
	DBusError error;
	DBusMessage *message;

	dbus_error_init(&error);
	message = dbus_message_new_method_call(DBUS_SERVICE_DBUS,
										   DBUS_PATH_DBUS,
										   DBUS_INTERFACE_DBUS,
										   "GetNameOwner");
	if (message == NULL)
		return veFalse;

	if (!dbus_message_append_args(message, DBUS_TYPE_STRING,
								  &remoteService->serviceName, DBUS_TYPE_INVALID)) {
		dbus_message_unref(message);
		return veFalse;
	}

	info("get owner of %s", remoteService->serviceName);
	veDbusSendWithReply(remoteService->conn, message, gotNameOwner, remoteService, block);

	return veTrue;
}

static DBusHandlerResult handleListeningServices(DBusConnection *connection,
									DBusMessage *msg, void *data)
{
	struct VeRemoteService *remote = (VeRemoteService *) data;
	const char *path;
	struct VeItem *item;
	DBusMessageIter args, sub;
	int currentType;
	char const *sender;
	VE_UNUSED(connection);

	/* only match the correct dbus service for incoming property changes */
	sender = dbus_message_get_sender(msg);
	if (remote->uniqueName == NULL || sender == NULL ||
			strcmp(remote->uniqueName, sender) != 0 ||
			!dbus_message_is_signal(msg, veInterface, "PropertiesChanged"))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	path = dbus_message_get_path(msg);
	item = veDbusItemByUid(remote->items, path);

	if (!item)
		goto out;

	if (!dbus_message_iter_init(msg, &args))
		goto out;

	if (!dbus_message_iter_init(msg, &args) || dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_ARRAY)
			goto out;

	/* walk through the array */
	dbus_message_iter_recurse(&args, &sub);
	while ((currentType = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID)
	{
		char *property;
		DBusMessageIter entry;

		dbus_message_iter_recurse(&sub, &entry);
		dbus_message_iter_get_basic(&entry, &property);
		dbus_message_iter_next(&entry);

		/* Handle the Value property */
		if (strcmp(property, "Value") == 0) {
			VeVariant var;

			veItemLocalValue(item, &var);

			/*
			 * Pass in a copy of the type of the variant, so the read routine can
			 * actually check if it is of the valid type / perform a conversion.
			 * Don't pass in the current value though, so it can never be freed
			 * by accident.
			 */
			var.value.Ptr = NULL;

			if (!veDbusMsgReadVeVariant(&entry, &var))
				goto out;
			veItemOwnerSet(item, &var);
		}
		dbus_message_iter_next(&sub);
	}

out:
	return DBUS_HANDLER_RESULT_HANDLED;
}

/*
 * Induce a change value request on the dbus. If the value is allocated
 * it is freed here. Once accepted by the remote side it should be confirmed
 * with a PropertiesChanged signal as handled above, which does the actual
 * veItemOwnerSet. iow there will be a valueChanged callback _after_ the remote
 * side confirmed the value is accepted.
 */
static veBool forwardSetValue(struct VeItem *item, void *ctx, VeVariant *variant)
{
	DBusMessage *msg;
	DBusMessageIter itt;
	char buf[VE_MAX_UID_SIZE];
	struct VeRemoteService *remote =  (struct VeRemoteService *) veItemDbus(item);
	VE_UNUSED(ctx);

	if (veDbusItemPath(item, buf, sizeof(buf)) >= sizeof(buf))
		return veFalse;

	msg = dbus_message_new_method_call(remote->uniqueName, buf,
											veInterface, "SetValue");
	if (!msg)
		return veFalse;

	dbus_message_iter_init_append(msg, &itt);
	veDbusMsgAppendVeVariant(&itt, variant);
	veDbusSend(remote->conn, msg);

	/* free the message */
	dbus_message_unref(msg);

	veVariantFree(variant);

	return veTrue;
}

static void gotValue(DBusPendingCall *pending, void *ctx)
{
	DBusMessage *reply;
	struct VeItem *item = (struct VeItem *) ctx;
	VeVariant var;
	DBusMessageIter args;
	char const *errorString;

	reply = dbus_pending_call_steal_reply(pending);
	if (reply == NULL)
		return;

	errorString = dbus_message_get_error_name(reply);
	if (errorString) {
		error("gotvalue %s", errorString);
	} else {
		if (dbus_message_iter_init(reply, &args) && veDbusMsgReadVeVariant(&args, &var)) {
			veItemOwnerSet(item, &var);
		} else {
			error("invalid GetValue reply");
		}
	}

	dbus_message_unref(reply);
}

void forwardGetValue(struct VeItem *item)
{
	DBusMessage *msg;
	char buf[VE_MAX_UID_SIZE];
	struct VeRemoteService *dbus = (struct VeRemoteService *) veItemDbus(item);

	if (veDbusItemPath(item, buf, sizeof(buf)) >= sizeof(buf))
		return;

	msg = dbus_message_new_method_call(dbus->uniqueName, buf,
											veInterface, "GetValue");

	if (!msg)
		return;

	veDbusSendWithReply(dbus->conn, msg, gotValue, item, veFalse);

	/* free the message */
	dbus_message_unref(msg);
}

static void onChildAdded(struct VeItem *childItem)
{
	struct VeItem *parent = veItemParent(childItem);
	struct VeDbus *dbus = veItemDbus(parent);

	setItemProperties(childItem, dbus);
}

static void onItemRemoved(struct VeItem *item)
{
	struct VeDbus *dbus = veItemDbus(item);
	if (dbus->items == item)
		dbus->items = NULL;
	veItemForeach(item, removeItemProperties, NULL);
}

/* register the onchange callback to forward changes to the dbus */
static void setItemProperties(struct VeItem *item, void *ctx)
{
	veAssert(veItemDbus(item) == NULL);

	veItemSetSetter(item, forwardSetValue, NULL);
	veItemSetGetter(item, forwardGetValue);
	veItemSetChildAdded(item, onChildAdded);
	veItemSetAboutToRemoved(item, onItemRemoved);
	veItemSetDbus(item, ctx);
}

static void removeItemProperties(struct VeItem *item, void *ctx)
{
	VE_UNUSED(ctx);

	veItemSetSetter(item, NULL, NULL);
	veItemSetGetter(item, NULL);
	veItemSetChildAdded(item, NULL);
	veItemSetAboutToRemoved(item, NULL);
	veItemSetDbus(item, NULL);
}

/*
 * Start listening to mentioned service and place received
 * values in dbusRoot. If block is set to true, this function
 * will return NULL if the service is not found on the bus.
 */
struct VeRemoteService *veDbusAddRemoteService(char const *serviceName, struct VeItem *dbusRoot, veBool block)
{
	struct VeRemoteService *remote = calloc(1, sizeof(VeRemoteService));
	VeStr s;
	DBusError error;

	veAssert(listeningDbus);

	remote->serviceName = strdup(serviceName);
	if (remote->serviceName == NULL)
		goto listen_error;
	remote->items = dbusRoot;
	remote->conn = listeningDbus->conn;

	if (!getNameOwner(remote, block))
		goto listen_error;

	/* When asked to block, return NULL when the service isn't found */
	if (block && remote->uniqueName == NULL)
		goto listen_error;

	if (!dbus_connection_add_filter(listeningDbus->conn, handleListeningServices, remote, NULL)) {
		error("dbus_connection_add_filter() failed");
		goto listen_error;
	}

	veStrNewFormat(&s, "type='signal',interface='" VE_INTERFACE "',sender='%s'", serviceName);
	if (s.error)
		goto listen_error;

	dbus_error_init(&error);
	dbus_bus_add_match(remote->conn, s.data, &error);
	veStrFree(&s);

	if (dbus_error_is_set(&error)) {
		error("dbus_bus_add_match failed");
		dbus_error_free(&error);
		goto listen_error;
	}

	/* Intercept calls to the item to forward them to the remote service */
	veItemForeach(dbusRoot, setItemProperties, remote);

	return remote;

listen_error:
	free(remote);
	return NULL;
}
