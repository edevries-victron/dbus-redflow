#ifndef _VELIB_J1939_J1939_DEVICE_H_
#define _VELIB_J1939_J1939_DEVICE_H_

#include <velib/base/compiler.h>

// note: Do not include velib/velib_inc_J1939.h here -> likely circular
// Forward declaration, velib_inc_J1939.h defines it. 
struct J1939Device;
struct J1939DeviceDef;

#if CFG_J1939_DEVICES > 1

// Multiple devices in this node.
VE_EXT struct J1939Device		j1939Devices[];
VE_EXT struct J1939Device*		j1939ActiveDevice;
#define j1939Device				(*j1939ActiveDevice)

VE_EXT struct J1939DeviceDef	j1939DevicesDef[];
VE_EXT struct J1939DeviceDef*	j1939ActiveDeviceDef;
#define j1939DeviceDef			(*j1939ActiveDeviceDef)

void j1939DevActivate(un8 n);
void j1939DevInit(void);

#else

VE_EXT struct J1939Device	j1939Device;

#define j1939DevActivate(n)

#endif

#endif

