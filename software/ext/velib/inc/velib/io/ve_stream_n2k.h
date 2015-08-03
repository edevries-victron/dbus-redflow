#ifndef _VELIB_IO_VE_STREAM_N2K_
#define _VELIB_IO_VE_STREAM_N2K_

/**
 * @defgroup VE_IO_STREAM_N2K IO-stream NMEA 2000
 * @{
 * @ingroup VE_IO
 * @brief
 * Like @ref VE_IO_STREAM but acting on incoming / outgoing NMEA 2000 message.
 * Furthermore some NMEA 2000 specific types are added. N2k utils add common interaction
 * with the stack itself.
 *
 * The code below illustrates how to create and send a message; It can be run on
 * Little and Big endian platforms and will not trap an exception if the larger 
 * integer fields are not byte aligned in the message.
 *
 * Contrary to what might be epected, the code size is not necessarily larger then
 * direct accessing the buffer itself (since the code to add and track position is reused, 
 * but actual code size depends ofcourse on pointer size and instruction set).
 *
 * The generic io functions are without doubt slower then platform specific ones. If 
 * this happens to be an issue the generic calls can ofcourse be replaced with more 
 * specific ones or even "inline".
 * 
 * @code
 *	veOutAllocSf();
 *	if (n2kMsgOut == NULL)
 *		return;
 *
 *	n2kMsgOut->msgId.w = J1939ID_PRI(6) | J1939ID_PGN(SOME_PGN);
 *
 *	// Add data
 *	veOutUn16Le(1);
 *	veOutUn8(3);
 *	veOutSn16Le(-10);
 *	veOutBits16Le(0x07, 13);
 *	veOutBits8Le(0x01, 2);
 *	veOutBits8Le(0x02, 3);
 *
 *	// Queue for sending (can't fail).
 *	veOutSendSf();
 *
 * @endcode
 */

#include <velib/io/ve_vstream.h>
#include <velib/J1939/J1939_types.h>

VE_DCL	veBool		veOutAllocSf(void);
VE_DCL	veBool		veOutAllocFp(void);
VE_DCL	void		veOutMsg(J1939Msg* msg);
VE_DCL	veBool		veOutFinalize(void);
VE_DCL	void		veOutPadding(void);
VE_DCL	void 		veOutN2kStr(char const* str);
VE_DCL	void 		veOutPgn(un8 datapage, un16 pgn);
VE_DCL	void		veOutSendSf(void);
VE_DCL	void		veOutSendFp(un8* seq);

VE_DCL	void		veInMsg(J1939Msg const* msg);
VE_DCL	un8			veInPgn(un16 *pgn);
VE_DCL	char const*	veInStrN2k(void);

/// The current input message being read.
VE_EXT	J1939Msg const* n2kMsgIn;

/// The current output message to be constructed. NULL if allocation failed.
VE_EXT	J1939Msg* n2kMsgOut;

/**
 * The stream info as expected by @ref VE_IO_STREAM. It equals @ref veStreamOut
 * during the construction of a messages but does not need to be derefenced.
 */
VE_EXT	VeStream n2kStreamOut;

VE_EXT	VeStream n2kStreamIn;
/// @}

#endif
