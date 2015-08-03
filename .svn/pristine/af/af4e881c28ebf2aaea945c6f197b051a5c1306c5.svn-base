#ifndef _VELIB_FLASH_HCS08_
#define _VELIB_FLASH_HCS08_

veBool flashWriteInRam(un8 const * src, un8 length, un8* tg, un8 cmd);

/* note: Just write a dummie value */
#define flashErase(tg)											\
	flashWriteInRam(&RTCCNT, 1, (tg), mSectorErase)

#define flashWrite(src, length, tg)								\
	flashWriteInRam((src), (length), (tg), mBurstProg)

#endif
