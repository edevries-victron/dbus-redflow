#include "platform.h"
#include <velib/base/base.h>

void memorySelect(un8 bank)
{
	//Select eeprom page
	FCNFG_EPGSEL=bank;
}

#pragma CODE_SEG RAM_FUNCTION
veBool flashWriteInRam(un8 const * src, un8 length, un8* tg, un8 cmd)
{
	un8 error = 0;
	
	/* Clear FPVIOL & FACCERR flags */
	FSTAT = FSTAT_FPVIOL_MASK | FSTAT_FACCERR_MASK;
	while (length--)
	{
		FSTAT_FACCERR = 0;			/* clear error */
		while (!FSTAT_FCBEF);		/* Wait until ready to accept command */
		*tg++ = *src++;  			/* Write RAM data to FLASH address */
		FCMD = cmd;					/* program command (burst, erase etc) */
		FSTAT_FCBEF = 1;			/* Initiate command */
		__RESET_WATCHDOG();			/* Delay at least 4 cycles */
		error = FSTAT & (FSTAT_FPVIOL_MASK | FSTAT_FACCERR_MASK);
		if (error)
			break;
	}
	while (!FSTAT_FCCF);			/* Wait for command complete */
	return error;
}
#pragma CODE_SEG default
