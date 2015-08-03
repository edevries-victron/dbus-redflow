#ifndef _BOOT_VERIFY_H_
#define _BOOT_VERIFY_H_

typedef enum
{
	VERIFY_BUSY,
	VERIFY_PASSED,
	VERIFY_FAILED
} VerifyState;

extern VerifyState verifyState;
// can be used to monitor progress but not always needed.
extern un8 verifyProgress;

void verifyInit(void);
void verifyUpdate(void);

#endif
