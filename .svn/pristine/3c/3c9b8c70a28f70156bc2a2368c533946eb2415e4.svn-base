/** 
* @file
*    The flash write module stores the received data in flash.
*/

#include <string.h>

#include <velib/vecan/reg.h>

/**
*/
void progBegin(void)
{
}

/**
*/
VeRegAckCode progData(void)
{
	return VACK_ERR_BUSY;
}

/* Handle data / poll for error */
VeRegAckCode progUpdate(void)
{
	return VACK_ERR_INVALID;
}
