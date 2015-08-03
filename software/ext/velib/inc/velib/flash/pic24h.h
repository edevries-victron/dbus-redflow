#ifndef _VELIB_FLASH_PIC24H_
#define _VELIB_FLASH_PIC24H_

// byte counter
#ifdef WIN32
typedef char const* FlashDataPtr;
#else
typedef un16 FlashDataPtr;
#endif

void flashErase(FlashDataPtr offset);
veBool flashWriteData(un8 const * src, un16 length, FlashDataPtr tg);

#endif
