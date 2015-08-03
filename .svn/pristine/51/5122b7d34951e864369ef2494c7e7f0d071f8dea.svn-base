#include "p24Hxxxx.h"
#include <velib/base/types.h>
#include <velib/flash/pic24h.h>

/* 
* Flash write routines for the PIC24H family.
*
* @details
*
* Recall, CODE is ordered as:
*
*				 |----|----|----|
*				 | HL | LH | LL |
*			|----|----|----|----|
*	0x0000	| @0 |    instr     |
*	0x0002	| @0 | .. | .. | .. |
*	0x0004	| @0 | .. | .. | .. |
*			|----|----|----|----|
*
* with 
*	instr	Some 24bit instruction.
*	@0		The phantom byte, which always reads 0.
*
*	A single instruction takes 2 words of which one misses a byte.
*	The whole space is called an instruction word, just to confuse I guess...
*	Both words are Little Endian.. As byte array it looks like:
*
*	0x0000	| LL | 
*			| LH |
*	0x0001	| HL | 
*			| @0 |
*	0x0002	| LL |
*			| LH |
*	0x0003  | HL | 
*			| @0 |
*
* CONST DATA is stored in flash as:
*
*				 |----|----|----|
*				 | HL | LH | LL |
*			|----|----|----|----|
*	0x0000	| @0 | ## | DH | DL |
*	0x0002	| @0 | ## | .. | .. |
*	0x0004	| @0 | ## | .. | .. |
*			|----|----|----|----|
*
*	## not used byte... 
*
*	Data is visible in the memory (in LE un16) as:
*	 		|----|----|
*	0x8000	| DH | DL |
*	0x8002	| DH | DL |
*	 		|----|----|
*
*  Or as bytes:
*	0x8000	| DL |
*	0x8001	| DH |
*	0x8002	| DL |
*	0x8003	| DH |
*
* A PIC24H cannot program bytes, only the words of an instruction word (mmm),
* therefore 0xFF is used for writing to unused locations.
* To simplify programming data to flash a bit, the same numbering is used for 
* flash as for the flash seen in memory (PSV); and as an odd consequence 
* the offset must be set to the word location and relative position is in bytes.
*
* So when programming 123		to 0x200, the following is done:
*
*	     		 |----|----|----|
*	     		 | HL | LH | LL |
*	 		|----|----|----|----|
*	0x0200	| @0 | *0 | 2  | 1  |
*	0x0202	| @0 | *0 | FF | 3  |
*	0x0204	| @0 | *0 | .. | .. |
*			|----|----|----|----|
*
*	FlashDataPtr	| 	PSV			|	 |
*	----------------|---------------|----|
*	0x0200			|	0x8200		|  1 |
*	0x0201			|	0x8201		|  2 |
*	0x0202			|	0x8202		|  3 |
*	0x0203			|	0x8203		| FF |
*
*/

static void flashCommit(void)
{
	// Disable Interrupts for 5 Instructions (unlock)
	asm volatile ("disi #5");
	
	// Issue Unlock Sequence & Start Write Cycle
	__builtin_write_NVM();
	while(NVMCONbits.WR) ;
}

/* Erase a full 0x400 words, 0x200 instruction, 768 bytes page.
*/
void flashErase(FlashDataPtr offset)
{
	NVMCON = 0x4042;  // Perform Erase of a Full Page (8 rows)
	TBLPAG = 0;
	__builtin_tblwtl(offset, 0);
	flashCommit();
} 

/**
* Writes CONST DATA from src to tg in flash.
*
* @param src 		data to program to flash
* @param length		number of bytes to program
* @param tg			location where to program.
*
* @retval 			wether successful
*
* Target location uses addressing like PSV. 
*
*	flashWriteData("1234", 4, 0x200) will make "1234" in PSV at 0x8200.
*	It is actually stored in words 0x200 and 0x202. 0x201 and 0x203 are skipped.
*
*	flashWriteData("123", 3, 0x200) 
*	flashWriteData("456", 3, 0x203) 
*	Will set "123456" in PSV at 0x8200.
*	The word 0x202 is written to twice but with 0xFF on unused places.
*
*	@note
*		There is a default function to copy the data out of flash to memory if not
*		in PSV, memcpy_p2d16.
*	@note
*		For sake of completeness, to area written to must ofcourse be 0xFF.
*/
veBool flashWriteData(un8 const * src, un16 length, FlashDataPtr tg)
{
	un16 val;

	NVMCON = 0x4003;  // Perform Write of 1 word	
	TBLPAG = 0;
	
	// Write the first byte seperatly if it is halfway a word to the
	// MSB of the low word.. The LSB might already have data in it.
	if (tg & 1)
	{
		val = (*src++ << 8) | 0x00FF;
		__builtin_tblwtl(tg, val);
		__builtin_tblwth(tg, 0xFFFF);
		flashCommit();

		if ((__builtin_tblrdl(tg) & 0xFF00) != (val & 0xFF00))
			return veFalse;
		tg++;
	}

	// Write the main part as words.
	while (length > 1)
	{
		val = (*(src+1) << 8) | *src;
		__builtin_tblwtl(tg, val);
		__builtin_tblwth(tg, 0xFFFF);		// 3rd byte is not used for data...
		flashCommit();

		if (__builtin_tblrdl(tg) != val)
			return veFalse;

		// Writing two bytes of data, needs 3 bytes of storage and uses 4 bytes /
		// 2 words in code addresses, 2 bytes in RAM.
		length -= 2;
		tg += 2;
		src += 2;
	}

	// Write the last byte seperatly if it is halfway the word.
	// Unused to 0xFF so it can be programmed later.
	if (length)
	{
		val = 0xFF00 | *src;
		__builtin_tblwtl(tg, val);
		__builtin_tblwth(tg, 0xFFFF);
		flashCommit();

		if ((__builtin_tblrdl(tg) & 0xFF) != (val & 0xFF))
			return veFalse;
	}

	return veTrue;
}
