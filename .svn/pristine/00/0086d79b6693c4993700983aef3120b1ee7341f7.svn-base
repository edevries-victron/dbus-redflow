#include <velib/base/base.h>
#include <velib/utils/ve_event.h>
#include <velib/utils/ve_todo.h>

/**
* In order to keep response times short on a none pre-emptive platform,
* update / timer function can better be executed one at a time, instead
* of all in a row, allowing time critical functions to be served 
* inbetween.
*
* Below is the simplest (and dummest) implementation I can think of;
* looping one at a time. Timer progress goes in favor of normal updates.
* Since the init functions start with calling all handlers with tick events,
* there is no need to make seperate functionality for intervals, init can 
* just be called again.
*
* Looping is done through a function pointer list, veEventHndlrDef[], 
* which must be defined by the application. The list must be ended by NULL.
*
* veEventsInit -> normally called on startup and after intervals
* veEventsUpdate -> normaly called from mainloop
*/

static VeEventHndlr const* hdnlr;
static veBool tick;

#if CFG_UTILS_TODO_ENABLED
veBool allDone;
#endif

void veEventInit(void)
{
	hdnlr = veEventHndlrDef;
	tick = veTrue;
}

void veEventUpdate(void)
{
	(*hdnlr++)(tick ? VE_EV_TICK : VE_EV_UPDATE);

#if CFG_UTILS_TODO_ENABLED
	if (veIsTodo())
		allDone = veFalse;
#endif

	// check end of list
	if (*hdnlr != NULL) {
#if CFG_UTILS_TODO_ENABLED
		veTodo();
#endif
		return;
	}

#if CFG_UTILS_TODO_ENABLED
	if (tick || allDone != veTrue)
		veTodo();
	allDone = veTrue;
#endif

	// 	resume normal update
	hdnlr = veEventHndlrDef;
	tick = veFalse;
}
