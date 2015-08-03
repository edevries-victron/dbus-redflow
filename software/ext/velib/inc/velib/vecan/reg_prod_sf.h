#ifndef _VELIB_VECAN_VE_REG_PROD_SF_H_
#define _VELIB_VECAN_VE_REG_PROD_SF_H_

#include <velib/vecan/reg.h>
#include <velib/types/types.h>
#include <velib/utils/ve_events.h>
#include <velib/types/variant.h>

typedef un8 VeRegNr;
#define VE_REG_NR_INVALID 0xFF

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_REG_PROD_DATA Sending VRegs (SF)
 */
/// @{

/* Default: vregs are only sent on request */
#define V_VREQ					0x00

/* This is the default slow background update */
#define V_PERIODIC				0x01

/* Not defined, but different timers frequencies can be used...*/
//#define V_PERIODIC_FAST			0x02

/* Space reserved for different timers */
#define V_TIMER_MASK			0x01

/* When set the values is obtained by a function pointer instead of a data pointer */
#define V_FUN					0x40

#define V_WRITE					0x80

#include <velib/vecan/reg_prod_sf_types.h>

typedef un8 VeIo;
#define V_IN					1
#define V_OUT					0
#define V_GET					V_OUT
#define V_SET					V_IN
typedef VeRegAckCode VeRegProdValueCallback(VeIo io, VeRegProdSfTypedDef const* vreg, VeVariant* variant);

/// Memory which must be provided by the application to mark messages.
/// Size in bytes must equal the number of messages.
extern un8 veRegProdSfFlags[];


/// Messages to send, to be defined by the application
VE_EXT	VeRegProdSfDef const veRegProdSfDef[];

VE_EXT	VeRegProdSfTypedDef const veRegProdSfTypedDef[];

void	veRegProdSfInit(void);
void	veRegProdSfUpdate(void);
void	veRegProdSfHandler(VeEvent ev);
void	veRegProdSfReq(VeRegId regId, un16 mask);
void 	veRegProdSfReqVreg(VeRegId regId);
void	veRegProdSfModifyFlag(VeRegId regId, un8 flag, veBool value);
void	veRegProdSfTick(void);

void	veRegPingHandler(VeEvent ev);

void	veRegOnChange(void* tg);
void	veRegOnChangeNr(VeRegNr n);
void	veRegOnChangeId(VeRegId regId);
veBool	veRegSetUn8(un8* tg, un8 value);
veBool	veRegSetUn16(un16* tg, un16 value);
veBool	veRegSetUn32(un32* tg, un32 value);

/** some predefined vregs */

/* a bit ugly, but useful to just add the error counters as vregs */
#define VE_REG_PROD_DEF_CAN_BASIC_ERRORS																								\
	{VE_REG_RX_HW_OVERFLOW_COUNT,	&veCanBusStats.hwRxOverflow,	sizeof(veCanBusStats.hwRxOverflow),	BUS_STAT_COUNT_TYPE,	0},		\
	{VE_REG_RX_SW_OVERFLOW_COUNT,	&veCanBusStats.sfRxOverflow,	sizeof(veCanBusStats.sfRxOverflow),	BUS_STAT_COUNT_TYPE,	0},		\
	{VE_REG_ERROR_PASSIVE_COUNT,	&veCanBusStats.errorPassive,	sizeof(veCanBusStats.errorPassive),	BUS_STAT_COUNT_TYPE,	0},		\
	{VE_REG_BUSOFF_COUNT,			&veCanBusStats.errorBusOff,		sizeof(veCanBusStats.errorBusOff),	BUS_STAT_COUNT_TYPE,	0}		\

/* gcc will warn if not filled in completely with -Wall */
#define VE_REG_PROD_DEF_END																												\
	{0,								(void*) 0,						0,									0,						0}		\

/// @}

#endif
