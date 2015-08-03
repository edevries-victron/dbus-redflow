/**
* A test programmer, expecting continuesly increasing data.
* An error will be raised if the data is not as expected.
* The test is usefull for checking the transport layer.
*/

#include <velib/base/base.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/boot/prog.h>


static VeRegAckCode error;
static un8 looper;

/**
*/
void progDataInit(void)
{
	error = VACK_OK;
	looper = 0;
}

/**
*/
void progData(void)
{
	un8 const* data = veInPtr();
	un8 length = veInBytesLeft();
	un8 n;

	for (n = 0; n < length; n++)
	{
		if (*data++ != looper)
		{
			error = VACK_ERR_INVALID | looper;
			return;
		}
		looper++;
	}
}

/* Handle data / poll for error */
VeRegAckCode progUpdate(void)
{
	return error;
}
