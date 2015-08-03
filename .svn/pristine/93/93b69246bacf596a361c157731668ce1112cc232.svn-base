#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_acl.h>
#include <velib/J1939/J1939_device.h>
#include <velib/io/ve_stream_n2k.h>

/** 
 * Handles an incoming commanded address message (CMDA). 
 * This method will check if it is directed to the current selected device
 * and change the address if so.
 *
 * The CMDA is send by an BAM, see @ref VELIB_J1939_TRANSPORT or by the
 * smaller @ref VELIB_NMEA2K_FP_CMDA.
 */
void j1939DataCmdNad(void)
{
	J1939Name name;

	if (n2kMsgIn->length != 9)
		return;

	j1939AclReadName(&name);

	if (j1939AclCompare(&j1939Device.name, &name) == 0)
		j1939AclChangeNad(n2kMsgIn->mdata[8]);
}
