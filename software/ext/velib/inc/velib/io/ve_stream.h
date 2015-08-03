#ifndef _VELIB_IO_VE_STREAM_H_
#define _VELIB_IO_VE_STREAM_H_

/**
 * @defgroup VE_IO_STREAM IO-stream
 * @{
 * @ingroup VE_IO
 * @brief 
 *	Platform independent read / write of common types to / from a buffer
 *	without Endianess and alignment problems and checks for overflow.
 *
 *	Values returned and passed are in local Endianness. The stream can be 
 *	either Little or Big Endian or combined for fields aligned on a byte 
 *	boundary. Bit fields of different endianess which are not byte aligned
 *	cannot be combined! There is per definition a problem since the parts 
 *	of the field are placed at different positions (Big Endian start at bit 7).
 *	The bit numbering for both Endianess differs in this implementation. 
 *	bitsUsed for little endian count from the right, for big endian from the
 *	left.
 *
 *	The indirect access to the buffer are added to easily allow configurable 
 *	message without causing severe problems and makes access to buffers the 
 *	same on all platforms (no acception will be trap when accessing odd aligned
 *	integers for example. The raw buffer can however be directly accessed by 
 *	veStreamIn->buffer might there be a need to do so.
 *
 *	The functions are divided over several files to easily include only Big 
 *	Endian or Little Endian support for example. Defines can be used to remove
 *	specific functionality to reduce code size if there is a need to.
 */

#include <velib/velib_config.h>
#include <velib/base/base.h>

typedef 	un16	VeBufSize;

typedef struct
{
	un8* 		buffer;		///< the actual data.
	VeBufSize	length;		///< the (used) length of the buffer.
	VeBufSize	index;		///< current byte position.
#ifndef CFG_VE_BITS_DISABLE
	un8			bitsUsed;	///< bit position, from left for Big Endian, right for Little.
#endif
	veBool		error;		///< veTrue when and overflow / underflow was encountered.
} VeStream;

/// Direct access to the input stream.
VE_EXT					VeStream* veStreamIn;
/// Direct access to the output stream.
VE_EXT					VeStream* veStreamOut;

//===== INPUT ======
// Common
VE_DCL void 			veInStream(VeStream* stream);
VE_DCL VeStream const*	veInStreamInit(VeStream* stream, un8* buffer, VeBufSize length);
VE_DCL void				veInRestore(VeStream const* stream);
VE_DCL void 			veInMove(VeBufSize byte);
VE_DCL void				veInMoveBit(un8 bitPos);
VE_DCL VeBufSize		veInBytesLeft(void);
VE_DCL veBool			veInUnderflow(void);

// independend		
VE_DCL un8 const* 		veInPtr(void);
VE_DCL un8 				veInUn8(void);
/// Read an sn8 and move ahead.
#define 				veInSn8() ((sn8) veInUn8())
VE_DCL	char const*		veInFixedString(VeBufSize length);

// Little endian
VE_DCL un16 			veInUn16Le(void);
/// Read an sn16 in Little Endian format and move ahead. 
#define					veInSn16Le() ((sn16) veInUn16Le())

VE_DCL un32				veInUn24Le(void);

#ifndef CFG_VE_IN_LE_UN32_DISABLE
VE_DCL un32 			veInUn32Le(void);
#define					veInSn32Le() ((sn32) veInUn32Le())
#endif

#ifdef CFG_VE_IN_LE_FLOAT_ENABLE 
VE_DCL float 			veInFloatLe(void);
#endif

// bits
#ifndef CFG_VE_IN_LE_BITS8_DISABLE
VE_DCL un8				veInBits8Le(sn8 nbits);
#endif

#ifndef CFG_VE_IN_LE_BITS16_DISABLE
VE_DCL un16				veInBits16Le(sn8 nbits);
#endif

#ifndef CFG_VE_IN_LE_BITS8_DISABLE
VE_DCL un32				veInBits32Le(sn8 nbits);
#endif

// Big endian
VE_DCL un16 			veInUn16Be(void);
VE_DCL un32 			veInUn32Be(void);
//VE_DCL float 			veInFloatBe(void);

// bits
VE_DCL un8 				veInBits8Be(sn8 nbits);
VE_DCL sn8				veInBits8BeSigned(sn8 nbits);
VE_DCL un16 			veInBits16Be(sn8 nbits);
VE_DCL sn16 			veInBits16BeSigned(sn8 nbits);
VE_DCL un32				veInBits32Be(sn8 nbits);

// ====== OUTPUT =======
// Common
VE_DCL	void 			veOutStream(VeStream* buf);
VE_DCL  VeStream const* veOutStreamInit(VeStream* stream, un8* buffer, VeBufSize length);
VE_DCL	void			veOutRestore(VeStream const* stream);
/// Current byte position in the output message.
#define					veOutPos()		veStreamOut->index
VE_DCL	VeBufSize		veOutBytesLeft(void);
VE_DCL	veBool			veOutOverflow(void);
VE_DCL	void			veOutMove(VeBufSize byte);
VE_DCL	void			veOutMoveBit(un8 bitPos);

// independend
VE_DCL	void 			veOutUn8(un8 value);
/// Add a sn8 and move ahead.
#define 				veOutSn8(value) veOutUn8((un8) (value))
VE_DCL	un8 * 			veOutPtr(void);
VE_DCL	void			veOutStr(char const * const str);
VE_DCL	void			veOutBuf(un8 const * buf, un8 len);

// Little endian
VE_DCL	void 			veOutUn16Le(un16 value);
/// Add a sn16 in Little Endian format and move ahead.
#define 				veOutSn16Le(value) veOutUn16Le((un16) (value))
VE_DCL	void 			veOutUn24Le(un32 value);
VE_DCL	void 			veOutUn32Le(un32 value);
#define					veOutSn32Le(value) veOutUn32Le((sn32) (value))

#ifdef CFG_VE_OUT_LE_FLOAT_ENABLE
VE_DCL	void			veOutFloatLe(float value);
#endif

// bits
VE_DCL	void			veOutBits8Le(un8 value, sn8 nbits);
VE_DCL	void			veOutBits16Le(un16 value, sn8 nbits);
VE_DCL	void			veOutBits32Le(un32 value, sn8 nbits);

// strings
VE_DCL	void			veOutFixedStr(char const * const str, un8 const maxlength);

// Big endian
VE_DCL	void 			veOutUn16Be(un16 value);
VE_DCL	void 			veOutUn32Be(un32 value);
//VE_DCL	void			veOutFloatBe(float value);
VE_DCL	void			veOutBits8Be(un8 value, sn8 nbits);
VE_DCL	void			veOutBits16Be(un16 value, sn8 nbits);
VE_DCL	void			veOutBits32Be(un32 value, sn8 nbits);
VE_DCL	void			veOutBits16BeSigned(sn16 value, sn8 nbits);

/// @}

#endif
