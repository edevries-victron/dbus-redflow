#ifndef _VELIB_TYPES_VE_DBUS_INTERNAL_H_
#define _VELIB_TYPES_VE_DBUS_INTERNAL_H_

#include <velib/velib_config.h>

#if CFG_WITH_LIBEVENT
#include <event2/event.h>
#include <event2/event_struct.h>
#endif

typedef struct VeDbus {
	/* housekeeping */
	struct VeDbus *next;

	/* The dbus service connection associated with this service */
	DBusConnection *conn;
	/*
	 * The root of the dbus items exported in this service.
	 * This is not per definition the root of the tree, since the
	 * export might be limited to a subtree.
	 */
	struct VeItem *items;
	/* Dbus serviceName associated with this service. */
	char *serviceName;

#if CFG_WITH_LIBEVENT
	/* Async event notifier */
	struct event dispatchEvent;
#endif
} VeDbus;

typedef struct VeRemoteService {
	/* housekeeping */
	struct VeRemoteService *next;
	/* connection access the dbus for this service (always default for now) */
	DBusConnection *conn;
	/*
	 * The root of the dbus items imported from this service.
	 * This is not per definition the root of the tree, since the
	 * export might be limited to a subtree.
	 */
	struct VeItem *items;
	/* Dbus serviceName associated with remote service. */
	char *serviceName;
	char *uniqueName;
} VeRemoteService;

#define VE_INTERFACE		 "com.victronenergy.BusItem"
static char const *veInterface = VE_INTERFACE;

veBool veDbusMsgAppendVeVariant(DBusMessageIter *itt, VeVariant *var);
veBool veDbusMsgReadVeVariant(DBusMessageIter *itt, VeVariant *variant);
size_t veDbusItemPath(struct VeItem *item, char *path, size_t len);
struct VeItem *veDbusItemByUid(struct VeItem *dbusRoot, char const *uid);
veBool veDbusSend(DBusConnection *conn, DBusMessage* msg);
veBool veDbusSendWithReply(DBusConnection *conn, DBusMessage* msg,
						   DBusPendingCallNotifyFunction function, void *ctx, veBool hang);

#endif
