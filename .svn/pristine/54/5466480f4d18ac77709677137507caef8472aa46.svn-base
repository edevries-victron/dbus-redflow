#ifndef _VELIB_J1939_J1939_STACK_H_
#define _VELIB_J1939_J1939_STACK_H_

/**
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_STACK	Stack
 * @brief 
 *	The stack contains the variables used by the J13939 / NMEA 2000 transport protocols.
 * @details
 *	The library code uses j1939Stack as the active stack. There is normally 
 *	only one stack. If sitiations arrise where multiple stack are necessary, j1939Stack
 *  can be changed to point to the active stack.
 *
 *	The actual definition of J1939Stack should be defined in velib/velib_inc_J1939.h 
 *	depending on the transport protocols used.
 * @{
 */

// Forward declaration, exact members might depend on the application.
struct J1939Stack;
extern struct J1939Stack 	j1939Stack;

#if CFG_J1939_STACKS>1

typedef 		un8 J1939Handle;

void 			j1939SetActiveStack(J1939Handle handle);
J1939Handle 	j1939ActiveStack(void);

#endif // J1939_STACKS>1

/// @}

#endif

