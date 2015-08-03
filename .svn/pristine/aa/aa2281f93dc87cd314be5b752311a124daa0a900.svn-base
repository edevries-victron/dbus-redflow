#ifndef _VELIB_CHECKS_CRC_H_
#define _VELIB_CHECKS_CRC_H_

#include <velib/base/types.h>

/* lookup table */
extern un16 const crc16CcittTbl[];
extern un32 const crc32Tbl[];

/* CRC-CCITT = x^16 + x^12 + x^5 + 1
 *
 * The x^n represents the n'th bit from the most significant bit (reflected), 
 * making the binary representation of the polynominal 0x8408. Note that XMODEM
 * and others use the same polynominal, but not reflected / different initial 
 * values and might be listed in validators and at times labeled as variants of 
 * CRC-CCITT. To makes matters worse, properties of the variants of the 
 * polynominal end up in CRC-CCITT making these "verification" flawed. Hence
 * some checks for the checks (note: without final inversion):
 * 
 * CRC-CCITT of ""				0xFFFF 
 * CRC-CCITT of "A"				0x5C0A
 * CRC-CCITT of "123456789"		0x6f91
 *
 * The CRC-CCITT is used in Irda and hence VE.Net. The logical inverted CRC is
 * appended (Little Endian). Calculating the CRC over the whole range becomes
 * CRC16_CCITT_OKE when valid.
 */
#define CRC16_CCITT_INIT(crc)		crc = 0xFFFF
#define CRC16_CCITT_ADD(crc, byte)	crc = ((crc >> 8)^ crc16CcittTbl[(crc ^ (byte)) & 0xFF])
#define CRC16_CCITT_RESULT(crc)		(~crc)	// just a reminder to append 1's complement
#define CRC16_CCITT_RESIDUE			0xF0B8	// should be left after sum including result.

/** 
 * The CRC32 is like CRC-CCITT, but implementation are more consistent,
 * likely because it is included in IEEE, hence it is just named crc32.
 * note, with final inversion; since this is in general included when revering
 * to a crc32 checksum.
 *
 * CRC32 of "123456789"				0xCBF43926
 */
#define CRC32_INIT(crc)				crc = 0xFFFFFFFF
#define CRC32_ADD(crc, byte)		crc = crc32Tbl[(crc ^ (byte)) & 0xFF] ^ (crc >> 8);
#define CRC32_RESULT(crc)			(~crc)
#define CRC32_RESIDUE				0xDEBB20E3

#endif
