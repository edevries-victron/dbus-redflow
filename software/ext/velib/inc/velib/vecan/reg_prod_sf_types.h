#ifndef _VELIB_VECAN_VE_REG_PROD_SF_TYPES_H_
#define _VELIB_VECAN_VE_REG_PROD_SF_TYPES_H_

/**
 * Format for defining a Victron SF proprietary broadcast message.
 * As used by the small version, reg_prod_sf.c
 */
typedef struct
{
	VeRegId 	regId;		///< Identifier determining the contents of message.
	un8 const*	data;		///< Data to send.
} VeRegProdSfDef;

/**
 * Format for defining a Victron SF proprietary broadcast message.
 * As used by the more luxures, but larger reg_prod.c.
 */
typedef struct
{
	VeRegId 		regId;		///< Identifier determining the contents of message.
	void*			data;		///< Data to send.
	un8				size;
	VeDataBasicType	datatype;
	un8				flags;
} VeRegProdSfTypedDef;

#endif
