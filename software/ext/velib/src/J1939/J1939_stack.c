#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/utils/ve_assert.h>

#if CFG_J1939_STACKS==1

struct J1939Stack j1939Stack;

#else

static J1939Handle j1939Handle;

// definition of all the stacks
J1939Stack j1939Stacks[CFG_J1939_STACKS];


/* Support for multiple stacks */
void j1939SetActiveStack(J1939Handle handle)
{
	veAssert(handle < CFG_J1939_STACKS);
	j1939Handle = handle;
}

J1939Handle j1939ActiveStack(void)
{
	return j1939Handle;
}

#endif

