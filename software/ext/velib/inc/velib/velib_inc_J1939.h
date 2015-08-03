#ifndef _VELIB_VELIB_INC_J1939_H_
#define _VELIB_VELIB_INC_J1939_H_

/**
 * While velib_config.h only defines constants, it is useful at times, to make
 * structure hold only the actual data used and not all the possible data.
 * Similiar function reference / constant might be referenced by defines
 * to allow a good amount of flexibility. And most important the application 
 * can easily tie additional information to functions / stacks etc when needed,
 * However adding everything to velib_config.h can easily lead to circular 
 * dependecies. Hence code is defined / added to the velib_inc_*.h files.
 *
 * This file should be included first by c-files. This file should not be 
 * included by the header file of velib itself. This file can then include 
 * the headers of velib without dependency problems.
 */

#include <velib/velib_config.h>

#if CFG_J1939_INC_APP
// When defined the application can define the basic types itself.
#include <velib/velib_inc_J1939_app.h>
#else
// By default a node with single frame, fp and ACL support and queues is defined.

// NOTE: since these headers are included here, they should not include this file!
#include <velib/J1939/J1939_nmt.h>
#include <velib/J1939/J1939_rx.h>
#include <velib/J1939/J1939_sf.h>
#include <velib/J1939/J1939_tx.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/vecan/udf_name.h>

/** 
 * Structure holding free message and queues etc. This must be defined.
 */
typedef struct J1939Stack
{
	J1939RxVars		rx;				// input queue
	J1939TxVars		tx;				// output queue
	J1939SfVars		sf;				// single frame 
	N2kFpVars		fp;				// fast packet
} J1939StackTp;

/// Contains information per Local Device Function (RAM).
typedef struct J1939Device
{
	/// NAME of the device
	J1939Name 		name;
	/// Device's current NAD (not per definition valid, see flags).
	J1939Nad		nad;
	/// Flags if NAD is valid etc.
	un8				flags;
	/// Variables for automatic address resolution.
	J1939AclVars	acl;
	/// UDF (update device function support)
	VeUdfVars		udf;
} J1939DeviceTp;

/** 
 * Structure for Data Consumers to store information of remote functions 
 * encountered on the bus.
 */
typedef struct J1939Func
{
	struct J1939Func*		next;
	J1939NmtVars			nmt;
} J1939FuncTp;

#endif

#endif
