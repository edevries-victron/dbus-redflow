#include <stdio.h>

#include <velib/velib_inc_mk2.h>
#include <velib/mk2/frame_handler.h>
#include <velib/utils/ve_logger.h>

/** debug functions for the pc */
void mk2DumpBuf(char* str, un8 const* data, int len)
{
	un8 i;
	char buf[1024];
	size_t n;
	char* p = buf;

	n = sprintf(p, "%s ", str);	p += n;
	for (i = 0; i< len; i++)
	{
		if (i == 2 && data[1] == 0xFF) {
			n = sprintf(p, "'%c ", data[2]);	p += n;
		} else {
			n = sprintf(p, "%02X ", (int) data[i]); p += n;
		}
	}

	logI("", buf);
}

void mk2Dump(char* str, Mk2MsgRx *frame)
{
	int len = frame->length + 2;
	if (len > 0x80)
		len -= 0x80;

	mk2DumpBuf(str, &frame->length, len);
}
