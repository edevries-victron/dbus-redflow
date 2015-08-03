/**
* @file
*
* @brief
*   Check for valid application in PIC24H by CRC.
*
* @details
*	The implementation assumes the following format of the program:
*
*	     |----|----|----|
*	     | H1 | LH | LL |
*	|----|----|----|----|
*	| #0 | .. |  instr  |
*	| #0 | .. | .. | .. |
*	| #0 | .. | .. | .. |
*	| #0 | 00 |   CRC   |
*	|----|----|----|----|
*
* with 
*	instr	The number of instructions the program contains.
*	CRC		The calculated 16 bit CRC value with the high byte 0.
*	#0		The phantom byte, which always reads 0.
*
* The following defines are required:
*
*	CFG_APP_PROG_START:
*		The start of the application program (in words).
*
*	CFG_APP_PROG_LEN_INSTR:
*		The number of instruction (3 bytes each) the program can 
*		consist of at maximum.
*
*	CRC
*		The CRC is calculated in un16's. This is not a requirement of
*		a CRC, but leads to a shorter implementation. An example is shown
*		below to illustrate how the CRC is calculated.
*
*	Considering the data (including the phantom byte, zero) is
*		03 00 00 00 03 02 01 00 CRC.L CRC.H 00 00
*
* As instructions, this is interpreted as:
*	     |----|----|----|
*	     | H1 | LH | LL |
*	|----|----|----|----|
*	| #0 | 00 | 00 | 03 |
*	| #0 | 01 | 02 | 03 |
*	| #0 | 00 |   CRC 	|
*	|----|----|----|----|
*
* For the CRC calculation as:
*	|----|----|
*	| H  | L  |
*	|----|----|
*	| 00 | 03 |
*	| #0 | 00 |
*	| 02 | 03 |
*	| #0 | 01 |
*	|   CRC   |
*	| #0 | 00 |
*	|----|----|
*
* Since the CRC is in Little Endian format, no byte swaps have to be performed. 
* The fact that it ends in a zero, makes sure no additional steps have to be 
* carried out to obtain the final CRC result.
*
* In short: The CRC is calculated of the Byte Swapped un16, of the 4 byte long 
* instructions (thus including the phantom byte). The CRC must be appended in
* Little Endian format, followed by 0.
*/

#include <p24Hxxxx.h>
#include <velib/velib_config.h>
#include <velib/base/base.h>
#include <velib/boot/verify.h>

VerifyState verifyState;

static un16 counter;
static un32 crcAddr;

/**
* @brief 
*	Initialise verification process.
*/
void verifyInit(void)
{
	un32 crcStoredPos;
	verifyState = VERIFY_BUSY;
	crcAddr = CFG_APP_PROG_START;

	// Catch the obvious problems:
	//  - a zero length (a single 0 will pass the test).
	// 	- an impossible length -> also catches 0xFFFF, i.o.w. empty flash.
	//  - the high byte of the last instruction (CRC) is not zero, i.o.w. 
	//    even if the flash was reprogrammed it apparently didn't reached the end.
	TBLPAG = crcAddr >> 16;
	counter = __builtin_tblrdl(crcAddr);
	if (counter == 0 || counter > CFG_APP_PROG_LEN_INSTR)
	{	 
		verifyState = VERIFY_FAILED;
		counter = 0;
	} else {
		// The pantom byte of the CRC should be 0
		crcStoredPos = crcAddr + 2 * ((un32) counter - 1);
		TBLPAG = crcStoredPos >> 16;
		if (__builtin_tblrdh(crcStoredPos) != 0) {
			verifyState = VERIFY_FAILED;
			counter = 0;
		}
	}

	// CRC init
	CRCWDAT = 0;					// NOTE: seed is zero (so valid CRC is 0 as well)
	CRCXOR = 0x1021; 				// 16 Bit CRC-CCITT Polynomial
	CRCCONbits.PLEN = 0x0F; 		// 16 bit CRC
	CRCCONbits.CRCGO = 1;
}

/**
* @brief
*	Update verification process.
* @details
*	Every call, the next instruction is read and the CRC is updated, 
*	until all firmware instructions are verified. verifyState will then
*	become either VERIFY_FAILED or VERIFY_PASSED.
*/
void verifyUpdate(void)
{
	if (counter == 0 || !CRCCONbits.CRCMPT)
		return;

	TBLPAG = crcAddr >> 16;
	CRCDAT = __builtin_tblrdl(crcAddr);
	CRCDAT = __builtin_tblrdh(crcAddr);
	crcAddr += 2;

	if (--counter == 0)
	{
		// note: The CRC already clocked through, by the trailing 0x0000.
		while(!CRCCONbits.CRCMPT);
		CRCCONbits.CRCGO = 0;
		verifyState = (CRCWDAT ? VERIFY_FAILED : VERIFY_PASSED);
	}
}
