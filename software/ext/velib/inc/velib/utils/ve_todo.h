#ifndef _VELIB_UTILS_TODO_H_
#define _VELIB_UTILS_TODO_H_

#include <velib/velib_config.h>

/**
 * @addtogroup VELIB_UTILS
 * @details
 *	<b>veTodo</b>
 *
 *	On event driver systems the veTodo should be called whenever more work
 *	can be done. When veTodo is not set the device can turn to sleep / low
 *	power mode until something changed. RX message, TX empty or timer tick.
 *	@ref CFG_UTILS_TODO_ENABLED must be defined to compile the veTodo's in.
 */
/// @{

#if CFG_UTILS_TODO_ENABLED
/**
 * Whenever more processing in the mainloop can be done the todo flag should be
 * set, in order to allow event driven systems to keep updating.
 */
VE_DCL void veTodo(void);
/// Returns if there is more todo.
VE_DCL veBool veIsTodo(void);
#else
#define veTodo()	do {} while (0)
#define veIsTodo()	do {} while (0)
#endif

/// @}

#endif
