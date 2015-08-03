#ifndef _VELIB_J1939_DEV_FUN_H_
#define _VELIB_J1939_DEV_FUN_H_

#include <velib/base/base.h>
#include <velib/J1939/J1939_acl.h>

// note: Do not include velib/velib_inc_J1939.h here -> likely circular
// Forward declaration, velib_inc_J1939.h defines it. 
struct J1939Func;

/// List of functional devices on the bus.
VE_EXT struct J1939Func* j1939FuncList;

struct J1939Func*	j1939FuncByName(J1939Name* name);
struct J1939Func*	j1939FuncByNad(J1939Nad nad);

VE_DCL	struct J1939Func*	j1939FuncAlloc(J1939Name* name);
VE_DCL	void				j1939FuncNew(struct J1939Func* func);

#endif
