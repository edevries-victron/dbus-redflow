#include <stdlib.h>

#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>
#include <velib/utils/ve_assert.h>

/**
 * @addtogroup VELIB_J1939_NMT
 *
 */

#if CFG_J1939_NMT_ENTRIES != 0
static J1939FuncTp nmtMem[CFG_J1939_NMT_ENTRIES];
static J1939FuncTp* nmtFree;
#endif

/**
 * @brief
 *	Initializes the free list for nmt database entries.
 * @details
 *	For the fixed size queue version the total number of entries must
 *	be set with @ref CFG_J1939_NMT_ENTRIES.
 * This value can be zero.
 */
void j1939NmtMemInit(void)
{
	un8 n = 0;

#if CFG_J1939_NMT_ENTRIES != 0
	nmtFree = NULL;
	while (n < CFG_J1939_NMT_ENTRIES)
	{
		nmtMem[n].next = (J1939FuncTp*) nmtFree;
		nmtFree = &nmtMem[n++];
	}
#endif
}

#if CFG_J1939_NMT_ENTRIES != 0

/**
 * @brief
 *	Allocates a nmt entry.
 *
 * @return
 *	The allocated entry or NULL if none available.
 */
void* j1939NmtAlloc(void)
{
	J1939FuncTp* ret;

	// check if there are free nmt storage slots
	// left in the free list
	if (nmtFree == NULL) {
		return NULL;
	}

	// get a nmt slot off the top of the free list
	ret = nmtFree;
	nmtFree = ret->next;

	ret->next = NULL;

	return (void*)ret;
}

/**
 * @brief Give a nmt entry back to the stack.
 */
void j1939NmtFree(void* nmt)
{
	veAssert(nmt != NULL);

	((J1939FuncTp*)nmt)->next = nmtFree;
	nmtFree = (J1939FuncTp*)nmt;
}

#endif