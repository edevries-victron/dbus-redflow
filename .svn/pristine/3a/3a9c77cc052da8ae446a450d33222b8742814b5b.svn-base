#ifndef _VELIB_TYPES_DBUS_ITEM_H_
#define _VELIB_TYPES_DBUS_ITEM_H_

#include <velib/types/ve_item.h>
#include <dbus/dbus.h>

struct VeDbus;
struct event_base;

struct VeDbus *veDbusConnect(DBusBusType dbusType);
struct VeDbus *veDbusConnectString(char const *address);
void veDbusSetDefaultConnectString(char const *address);
char const *veDbusGetDefaultConnectString(void);
struct VeDbus *veDbusGetDefaultBus(void);
veBool veDbusChangeName(struct VeDbus *dbus, char const *name);
void veDbusDisconnect(struct VeDbus *dbus);
char const *veDbusServiceName(struct VeDbus *dbus);

/* producer side */
void veDbusItemInit(struct VeDbus *dbus, struct VeItem *items);

/* consumer side */
void veDbusSetListeningDbus(struct VeDbus *dbus);
struct VeRemoteService *veDbusAddRemoteService(char const *serviceName, struct VeItem *dbusRoot, veBool block);

/* platform */
void veDbusItemUpdate(struct VeDbus *dbus);
void veDbusUseLibEvent(struct event_base *base);

#endif
