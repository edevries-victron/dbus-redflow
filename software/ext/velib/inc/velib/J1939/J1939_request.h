#ifndef _VELIB_J1939_J1939_REQUEST_H_
#define _VELIB_J1939_J1939_REQUEST_H_

#include <velib/J1939/J1939_acl.h>

J1939Msg* j1939RequestPgnMsg(J1939Nad tg, un8 dp, un16 pgn);
veBool j1939RequestPgn(J1939Nad tg, un8 dp, un16 pgn);

#endif
