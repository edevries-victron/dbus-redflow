#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939_device.h>

// By "magic" this becomes a pointer declaration of j1939ActiveDevice
// when CFG_J1939_DEVICES > 1. Otherwise it declares a single instance.
struct J1939Device 	j1939Device;

#if CFG_J1939_DEVICES > 1
// All devices.
struct J1939Device		j1939Devices[CFG_J1939_DEVICES];
struct J1939DeviceDef	j1939DeviceDef;

// Set the active device.
void j1939DevActivate(un8 n)
{
	j1939ActiveDevice = &j1939Devices[n];
	j1939ActiveDeviceDef = &j1939DevicesDef[n];
}

#endif
