#include <string.h> // memcpy

#include <velib/velib_inc_J1939.h>
#include <velib/velib_inc_vecan.h>

#include <velib/vecan/prod_data_cm.h>
#include <velib/vecan/prod_data.h>

#include <velib/nmea2k/n2k.h>
#include <velib/nmea2k/n2k_complex_rx.h>
#include <velib/types/variant.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/utils/ve_assert.h>

/**
 * @addtogroup VELIB_VECAN_PROD_DATA_CM
 * @brief
 *	Adds support for the @ref VELIB_NMEA2K_COMPLEX for all message defined in
 *	@ref VELIB_VECAN_PROD_DATA.
 * @details
 * 	The NMEA 2000 Complex Request provides means to query message with
 * 	particulair fields set to a certain values (e.g. the ACLs of Victron Devices
 *  only, only the Fuel Tanks and not the other types etc).
 * 
 * 	The NMEA 2000 Complex Command allows to command an arbitrary signal in an
 *	attempt	to change its value or invoke some action. Multiple signals in the 
 * 	same message can be changed at once. Instance fields are send along to 
 * 	select the correct instance of the message.
 *
 *	The NMEA 2000 Complex Write allows changing the instance fields themselves.
 * 
 * 	The field in these messages can be in an ARBRITARY order...
 * 
 * 	This module will not change any variable in the device by itself. If a value
 * 	made it through the sanity checks and is within the [min, max] of the signal
 *  @ref veProdDataChangeEvent will be called to indicated that signal is to be 
 *	set to the value as contained in variant. The application itself must do the 
 *	actual update since the pointed variable might actually be in flash etc and 
 *	might not be directly writable or temporarily unavailable.
 *
 *  Priority support should be set / removed by defining
 * 	@ref CFG_PROD_CM_PRIORITY_CHANGE.
 *
 *	
 *
 */
/// @{

/**
 * @internal
 * @brief
 * 	Reads the next request parameter from the input message and checks if this 
 * 	(still) matches with the passed message.
 * 
 * @param msgNr		The message being checked against.
 * @param signals	Corresponding signals of the checked message.
 *
 * @return			Error code or @ref N2K_CM_PARAM_NO_ERROR on success.
 * 
 * @details
 * 	Request parameters are appended to the Complex Request / Command and Write field
 * 	by their id (start at 1) to select the correct instance of the message. 
 * 
 * 	for example
 * 		- param 1 with un16 value 0x7F00
 * 		- param 3 with 4bit value 4
 * 		- param 7 with un8 value 0x60
 *
 * 	will be send as:
 * 		0x01 0x00 0x7F 0x03 0x04 0x07 0x060.
 *
 */
static N2kCmParamCode checkSelectParam(VeMsgNr msgNr, VeProdSgnDef const*signals)
{
	un8 sgnNr;
	VeProdSgnDef const	*signal;	/* Current signal. */
	VeVariant variant;
	un8 m;

	/* Read the values from the arguments. */
	sgnNr = veInUn8();
	if (sgnNr == 0)
		return N2K_CM_PARAM_INVALID;
	sgnNr--;

	/* Verify that the parameter is a valid index. */
	if (sgnNr >= VE_PROD_MSG_DEF_CONST(msgNr).number )
		return N2K_CM_PARAM_INVALID;
	
	/* Get the value from the buffer. */
	signal = &signals[sgnNr];
	variant.type.tp = signal->type;
	veInVariantLe(&variant);

	/* Check for valid read (no underflow). If a underflow is encountered
	 * the end of the message is reached and parsing can be stopped.
	 */
	if (veStreamIn->error)
		return N2K_CM_PARAM_INVALID;

	/*
	 * Something is seriously wrong if a transform is needed for a request
	 * parameter. This will result in a float value, which cannot be compared
	 * in a reliable way with the current value!
	 */
	if (signal->trans)
		return N2K_CM_PARAM_NOT_SUPPORTED;

	/* Bit fields are valid selectors as well. Some instance cover half a byte
	* and the ACL has fancy bit fields which might be used for querying as well. 
	*/
	if (veVariantIsBits(signal->type) > 0)
	{
		VeVariant variantBits;

		// Get the bit values and compare with passed bits.
		variantBits.type.tp = signal->type;
		veVariantFromBitBuffer(&variantBits, signal->value, signal->size);
		if (variantBits.value.UN32 == variant.value.UN32)
			return N2K_CM_PARAM_NO_ERROR;
		return N2K_CM_PARAM_OUT_OF_RANGE;
	}

	/* Only allow basic types (for now). */
	switch (signal->type)
	{
		case VE_UN8:
		case VE_SN8:
		case VE_UN16:
		case VE_SN16:
		case VE_UN24:
		case VE_SN24:
		case VE_UN32:
		case VE_SN32:
			break;

		default:
			return N2K_CM_PARAM_NOT_SUPPORTED;
	}

	/* Check if parameters equals the signal value.
	*
	* Since the passed value is in local endian and the value as well, the raw
	* bytes are simply compared. This implies however that the raw data of the
	* union overlaps the numerical values from 0. Compilers are not required to
	* do so as far am I aware, but all compilers used do. If this ever fails 
	* just get the buffer to a variant and make a typed comparison. 
	*/
	for (m = 0; m < signal->size; m++)
	{
		if (variant.value.Buffer[m] != ((un8*) signal->value)[m])
			return N2K_CM_PARAM_OUT_OF_RANGE;
	}

	return N2K_CM_PARAM_NO_ERROR;
}

/**
 * @internal
 * @brief
 * 	Updates signal only when variant is valid / within bound.
 * 
 * @details
 * 	Checks are performed on min / max and length etc. Actually changing the 
 * 	signal is done by calling @ref veProdDataChangeEvent, allowing the 
 * 	application to have a say in the change and the signal itself might be 
 * 	read only (setting in flash e.g.). For the signals which are in memory the
 * 	application can simply call @ref veProdDataCmSet.
 */
N2kCmParamCode safeUpate(VeProdSgnDef const* signal, VeVariant* variant)
{
	un8 length;
	VeVariant fVariant;

	/* Disallow writes to read-only arguments */
	if (!(signal->flags & S_WRITE))
		return N2K_CM_PARAM_ACCESS_DENIED;

	/* Type specific check, fixed string */
	length = veVariantIsCharArray(signal->type);
	if (length)
	{
		/* Fixed string are in general padded by spaces to fill up the 
		* remaining area. In order to store the string as a zero ended string,
		* these padded spaces are removed and storage can therefore be less
		* then the transport field.
		*/

		/* Find the length without the spaces or \0 at the end */
		while (length)			// length != 0, see above
		{
			length--;
			if (((char *)variant->value.Ptr)[length] != ' ' && ((char *)variant->value.Ptr)[length] != 0)
				break;
		}
		length++;				// pos -> length

		/* Make sure the remaing fits; an additional byte is needed for the \0, 
		* it must therefore be smaller!
		*/
		if (length >= signal->size)
			return N2K_CM_PARAM_OUT_OF_RANGE;

		variant->type.tp = VE_CHARN_SET(length);
	}
	
	/* Type specific check, NMEA string
	*/
	else if (signal->type == VE_STR_N2K)
	{
		/*
		* [length] [unicode] [char] [char] [..]
		* note: length < 2 would already have triggered an fatal error.
		* underflow as well. Not zero ended!
		*/
		if (((char *)variant->value.Ptr)[1] != 1)	/* not ASCII... ignore */
			return N2K_CM_PARAM_NOT_SUPPORTED;

		/* Make sure it fits, an additional byte is needed for the \0 */
		if ((((char *)variant->value.Ptr)[0] - 2) >= signal->size)
			return N2K_CM_PARAM_OUT_OF_RANGE;
	} 

	/* Type specific check, all 'numeric' formats */
	else 
	{
		VeProdSgnSpanDef const* span = veProdSgnSpanCallback(signal);

		/* Convert all values to float to check min / max */
		fVariant = *variant;
		veVariantToN32(&fVariant);


		/* Check numeric bounds */
		if (fVariant.type.tp == VE_UN32)
		{
			if (fVariant.value.UN32 < (un32) span->min || fVariant.value.UN32 > (un32) span->max)
				return N2K_CM_PARAM_OUT_OF_RANGE;
		} else {
			if (fVariant.value.SN32 < span->min || fVariant.value.SN32 > span->max)
				return N2K_CM_PARAM_OUT_OF_RANGE;
		}

		/** @todo Apply transform if needed 
		if (signal->trans)
		{
		}
		*/
	}

	/* Ok, made it; notify application of change request.
	 * Nothing has changed actually, the application must do that.
	 */
	return veProdDataChangeEvent(signal, variant);
}

/**
 * @internal
 * @brief
 *	Handles a incoming Complex Request.
 */
veBool veProdDataCmRequest(un8 dp, J1939Pgn pgn)
{
	un8					parameters;			/* number of parameters given in request */
	un8					n;
	veBool				found = veFalse;
	un8*				counter;
	veBool				isMatch;	
	VeMsgNr				msgNr;				/* index in the message definition */
	VeProdSgnDef const	*signals;			/* Array of singals of this message */
	N2kCmParamCode		paramCode;
	un32 				interval;
#if CFG_PROD_CM_INTERVAL_CHANGE
	veBool				store = veFalse;
#endif

	/* Get a frame for the ACK. If this fails we delay handling the frame.
	* The ACK message is set as output message.
	*/
	n2kCmPrepareAck(n2kMsgIn->msgId.p.src, dp, pgn);
	if (n2kMsgOut == NULL)
		return veFalse;

	/* Verify that the pgn is known	*/
	msgNr = VE_PROD_MSG_FIRST - 1;
	if (!veProdDataIsMsg(dp, pgn, &msgNr))
	{
		n2kCmSendAck(N2K_CM_PGN_NO_SUPPORT, N2K_CM_TRANS_NO_ERROR);
		return veTrue;
	}

	interval = veInUn32Le();

#if CFG_PROD_CM_INTERVAL_CHANGE
	veOutUn8(0);	/* no error */
#else
	if (interval != N2K_CM_REQ_INTERVAL_AS_IS)
		veOutUn8(N2K_CM_TRANS_PRI_INTERVAL_NOT_SUPPORTED << 4);
	else
		veOutUn8(0);	/* no error */
#endif

	/* Loop through the message instances. Send everything which matches:
	*  3.2.1.6. When requesting PGNs defined with required or optional Request Parameters
	*  the response will depend upon the specific PGN being requested (ignored), but in 
	*  general will evoke all possible responses of the given PGN.
	*/
	do
	{
		signals = &veProdSgnDef[VE_PROD_MSG_DEF_CONST(msgNr).signal];

		/* Build or rebuild the NACK */
		veOutMove(5);
		counter = veOutPtr();		/* keep pointer to counter */
		veOutUn8(0);				/* counter to zero */

		/* Extract arguments */
		veInMove(10);
		parameters = veInUn8();

		/* 0xFF indicates no parameters */
		if (parameters == 0xFF)
			parameters = 0;

		/* If there are no parameters at all, all instances match */
		isMatch = veTrue;

		for (n = 0; n < parameters; n++)
		{
			paramCode = checkSelectParam(msgNr, signals);

			veOutBits8Le((un8)paramCode, 4);
			(*counter)++;

			/* Stop looping through */
			if (paramCode != N2K_CM_PARAM_NO_ERROR)
			{
				isMatch = veFalse;
				break;
			}
		}

		/* Mark message for sending */
		if (isMatch == veTrue)
		{

#if CFG_PROD_CM_INTERVAL_CHANGE
			// Change the interval of each matching message.
			if (interval != N2K_CM_REQ_INTERVAL_AS_IS)
			{
				// Set new interval.
				veProdDataIntervalChangeEvent(msgNr, interval);
				store = veTrue;
			}
#endif

			// Send the message.
			veProdDataMarkMsg(msgNr);
			found = veTrue;
		}

	} while (veProdDataIsMsg(dp, pgn, &msgNr));

#if CFG_PROD_CM_INTERVAL_CHANGE
	// Only write the settings once..
	if (store)
		veProdDataStoreEvent();
#else
	// Small hack, also send the ACK if there is a interval error by pretending
	// that the message is not found.
	if (n2kMsgOut->mdata[4] != 0)
		found = veFalse;
#endif

	/* p44, 3.2.1.4 All devices shall be capable of acknowledging a Complex Request
	 * message addressed to it. The acknowledgement is the data requested or the 
	 * Expanded Acknowledgement .. 
	 */
	if (found == veFalse)
		n2kCmSendAddressed();	/* Only send if not broadcast. */
	else
		n2kFpTxFree((N2kFp*) n2kMsgOut);

	return veTrue;
}

/**
 * Sends a fatal error in response to a @ref N2K_CM_WRITE.
 *
 * The @ref N2K_CM_WRITE_REPLY does not have status fields but sends the new 
 * values back instead. When refering to a none existing pgn or parameter
 * @ref N2K_CM_WRITE_REPLY with no data at all is returned.
 */
static void veProdDataWriteError(void)
{
	veOutMove(5);
	veOutUn8(0);
	veOutUn8(0);
	n2kCmSendAddressed();	/* Only send if not broadcast. */
}

/**
 * @internal
 * @brief
 *	Handles a incoming Complex Write.
 *
 * @details
 *	Like a complex command, the function allows to change settings inside the 
 *	device. One difference is that it has a seperate set for selection a message
 *	and the new values. This allows changing the instance numbers of the message
 *	itself.
 *
 *	Another difference is that the normal ACK is not used; the new values are 
 *	returned instead. On a malformed request (unable to locate the correct 
 *	instance) a write reply is send with no data attached. If select fields are
 *	passed which map to multiple message only the first occurence is used.
 *
 *	Once the instance of the message is located, the new data will be send as far
 *	as the message can be parsed; if the first value is valid, but the next isn't
 *	only the first value is changed and the return message will only contain the
 *	new value of the first field.
 */
veBool veProdDataCmWrite(un8 dp, J1939Pgn pgn)
{
	VeMsgNr				msgNr;				/* Index in the message definition. */
	VeProdSgnDef const	*signals;			/* Array of signals of this message. */
	un8					paramId;			/* Signal index in the message (starting at 0). */
	VeProdSgnDef const	*signal;			/* Current signal. */
	VeVariant			variant;			/* The value being set. */

	veBool				store = veFalse;	/* Flag to indicate something was changed. */
	veBool				isMatch;
	un8*				counter;
	un8					selectPairs;
	un8					parameters;
	un8					n;
	
	n2kCmAlloc(n2kMsgIn->msgId.p.src, dp, pgn, N2K_CM_WRITE_REPLY);
	if (n2kMsgOut == NULL)
		return veFalse;

	/* The response is almost identical to the incoming write (just copy). */
	(void)memcpy(&n2kMsgOut->mdata[4], veInPtr(), veInBytesLeft());
	
	(void)veInUn8(); /* sequence, not used */
	selectPairs = veInUn8();
	parameters = veInUn8();

	/* Verify that the PGN is known. */
	msgNr = VE_PROD_MSG_FIRST - 1;
	if (veStreamIn->error || !veProdDataIsMsg(dp, pgn, &msgNr))
	{
		veProdDataWriteError(); /* Frees the frame. */
		return veTrue;
	}

	/* Try to locate the correct instance of the message. */
	do
	{
		isMatch = veTrue;
		veInMove(7);

		signals = &veProdSgnDef[VE_PROD_MSG_DEF_CONST(msgNr).signal];
		for (n = 0; n < selectPairs; n++)
		{
			if (checkSelectParam(msgNr, signals) != N2K_CM_PARAM_NO_ERROR)
			{
				isMatch = veFalse;
				break;
			}
		}
	} while(!isMatch && veProdDataIsMsg(dp, pgn, &msgNr));

	/* Not finding the PGN is fatal... */
	if (!isMatch)
	{
		veProdDataWriteError();	/* Frees the frame. */
		return veTrue;
	}

	/* From here it output (might) differ */
	veOutMove(veStreamIn->index);
	
	counter = &n2kMsgOut->mdata[6];
	*counter = 0;

	for (n=0; n<parameters; n++)
	{
		/* Read the values from the message. The passed id's have offset 1! */
		paramId = veInUn8();
		if (paramId != 0 && paramId <= VE_PROD_MSG_DEF_CONST(msgNr).number)
		{
			paramId--;
			signal = &signals[paramId];
			variant.type.tp = signal->type;
			veInVariantLe(&variant);

			/* Check for valid read (no underflow). If a underflow is encountered
			 * the end of the message is reached and parsing can be stopped.
			 */
			if (!veStreamIn->error)
			{
				if (safeUpate(signal, &variant) == N2K_CM_PARAM_NO_ERROR)
				{
					veProdDataMarkMsg(msgNr);
					store = veTrue;
				}
				
				/* Ignore errors, since the actual data is appended and then not changed.. */
				(*counter)++;
				veOutUn8(paramId+1);
				veProdDataOutSignal(signal);
			} else {
				break; /* Early end of stream -> fatal. */
			}
		} else {
			break; /* Unknown param -> fatal. */
		}
	}

	if (store)
		veProdDataStoreEvent();

	n2kCmSend();	/* Frees the frame. */
	return veTrue;
}

/**
 * @brief
 *	Handles a incoming Complex Command.
 *
 * @details
 *	A Complex Command is received for pgn. Msg already points to the arguments.
 *	A complex command can contain multiple parameters in the format
 *	2 3 4 5 6 with would mean 2 parameters, 3=4 and 5=6.
 *
 * @note
 *	- The parameters can occur in any order.
 *	- The size must of the value must be lookup in the definition of the signal.
 *	- NMEA signal indices start at 1!
 *	- Valid signals are always stored, also if additional non-valid signals
 *	  are passed along.
 * 	- Currently only a single instance of each PGN is supported.
 */
veBool veProdDataCmCommand(un8 dp, J1939Pgn pgn)
{
	VeMsgNr				msgNr;				// index in the message definition
	un8					parameters;			// number of parameters given in commad
	un8					paramID;			// signal index in the message (starting at 0)
	VeVariant			variant;			// Literal value and float version for bound checking
	VeProdSgnDef const	*signal;			// Current signal
	VeProdSgnDef const	*signals;			// Array of singals of this message
	un8*				counter;			// Pointer to the number of arguments in the ACK
	un8					n;	
	veBool				store = veFalse;	// Flag to indicate something was changed.
	N2kCmParamCode		paramCode;
	N2kCmPriCode		priority;

	/*	
	* p45: 3.2.2.3 This message shall only be addressed to a specific devices... 
	* If this is not the case the request is ignored.
	*/
	if (n2kMsgIn->msgId.p.ps == J1939_NAD_BROADCAST)
		return veTrue;
	
	/* Get a frame for the ACK. If this fails we delay handling the frame.
	* The ACK message is set as output message.
	*/
	n2kCmPrepareAck(n2kMsgIn->msgId.p.src, dp, pgn);
	if (n2kMsgOut == NULL)
		return veFalse;

	/* Verify that the pgn is known	*/
	msgNr = VE_PROD_MSG_FIRST - 1;
	if (!veProdDataIsMsg(dp, pgn, &msgNr))
	{
		n2kCmSendAck(N2K_CM_PGN_NO_SUPPORT, N2K_CM_TRANS_NO_ERROR);
		return veTrue;
	}

	// signal application for intended changed.
	veProdDataChangeBeginEvent(msgNr);

	signals = &veProdSgnDef[VE_PROD_MSG_DEF_CONST(msgNr).signal];

	priority = veInUn8() & 0x0F;				/* Priority */

#if CFG_PROD_CM_PRIORITY_CHANGE

	// Support for priority change.
	if (priority != N2K_CM_PRIORITY_UNCHANGED)
	{
		veOutUn8(veProdDataPriorityChangeEvent(msgNr, priority) << 4);
		store = veTrue;
	}
	else
		veOutUn8(0);							/* no error */

#else

	// No support for priority change -> error when attempted.
	if (priority == N2K_CM_PRIORITY_UNCHANGED)
		veOutUn8(0);							/* no error */
	else
		veOutUn8(N2K_CM_TRANS_PRI_INTERVAL_NOT_SUPPORTED << 4);

#endif

	counter = veOutPtr();						/* Keep pointer to counter. */
	veOutUn8(0);								/* Counter to zero. */

	/* Extract arguments */
	parameters = veInUn8();
	if (parameters == 0xFF)
		parameters = 0;

	for (n=0; n<parameters; n++)
	{
		/* Read the values from the message. */
		paramID = veInUn8() - 1;

		/* Verify that the parameter is a valid index. */
		if (paramID >= VE_PROD_MSG_DEF_CONST(msgNr).number )
		{
			veOutBits8Le(N2K_CM_PARAM_INVALID, 4);
			(*counter)++;
			continue;
		}
		
		signal = &signals[paramID];
		variant.type.tp = signal->type;
		veInVariantLe(&variant);

		/* Check for valid read (no underflow). If a underflow is encountered
		*  the end of the message is reached and parsing can be stopped
		*/
		if (veStreamIn->error)
		{
			paramCode = N2K_CM_PARAM_INVALID;
		}
		/* Ignore the selector fields. */
		else if (signal->flags & S_INSTANCE)
		{
			(*counter)++;
			veOutBits8Le(N2K_CM_PARAM_NO_ERROR, 4);
			continue;
		}
		/* Check bounds and update. */
		else
			paramCode = safeUpate(signal, &variant);

		/* Add ack code. */
		(*counter)++;
		veOutBits8Le((un8)paramCode, 4);

		/* Mark as successful storage. */
		if (paramCode == N2K_CM_PARAM_NO_ERROR)
		{
			store = veTrue;
			veProdDataMarkMsg(msgNr);
		}
	}

	if (store)
		veProdDataStoreEvent();

	n2kCmSend(); // the ack..

	return veTrue;
}

/**
 * Handles NMEA2000 complex group on input message.
 *
 * @ref N2K_PGN1_COMPLEX_GROUP should be passed here by @ref n2kMsgIn.
 */
veBool veProdDataMsg(void)
{
	J1939Pgn pgn;
	un8 dp;
	un8 function;

	veAssert(n2kMsgIn->type.p.kind == N2K_FP);
	veAssert(n2kMsgIn->msgId.p.pg.p.dp == 1 && (n2kMsgIn->msgId.p.pf == N2K_PGN1_PF_COMPLEX_GROUP));

	function = veInUn8();

	/* read PGN (this assumes all opcodes are followed by a pgn) */
	dp = veInPgn(&pgn);

	/* Send not supported back */
	if (!veStreamIn->error)
	{
		switch(function)
		{
			case N2K_CM_REQUEST:
				return veProdDataCmRequest(dp, pgn);

			case N2K_CM_COMMAND:
				return veProdDataCmCommand(dp, pgn);
			
			case N2K_CM_ACK:
				return veTrue;

			case N2K_CM_WRITE:
				return veProdDataCmWrite(dp, pgn);
		}
	}

	/* Send not supported back */
	if (!veStreamIn->error && n2kMsgIn->msgId.p.ps != J1939_NAD_BROADCAST)
		n2kCmSendNotImplemented(n2kMsgIn->msgId.p.src, dp, pgn);

	return veTrue;
}

/**
 * @brief
 * 	Updates the value of signal to the value of the variant.
 * @details
 * 	If the signal value is located in RAM this function can be used to update the
 * 	its value to the value of the variant. It is intended to be called from @ref 
 * 	veProdDataChangeEvent, at which time all sanity checks were already performed,
 *  hence the are not performed again.
 */
void veProdDataCmSet(VeProdSgnDef const* signal, VeVariant* variant)
{
	un8 length;

	/* set fixed length string but make it zero ended */
	length = veVariantIsCharArray(variant->type.tp);
	if (length)
	{
		(void)memcpy(signal->value, variant->value.Ptr, length);
		((un8*) signal->value)[length] = 0;
		return;
	}

	/* change bit in memory */
	if (veVariantIsBits(variant->type.tp))
	{
		veVariantToBitBuffer(variant, signal->value, signal->size);
		return;
	}

	switch(signal->type)
	{
	case VE_STR_N2K:
		// the length includes the first 2 opcodes.
		length = ((char *)variant->value.Ptr)[0] - 2;
		(void)memcpy(signal->value, &((char *)variant->value.Ptr)[2], length);
		((un8*) signal->value)[length] = 0;
		break;

	default:
		veVariantToByteBuffer(variant, signal->value, signal->size);
		break;
	}
}

/// @}
