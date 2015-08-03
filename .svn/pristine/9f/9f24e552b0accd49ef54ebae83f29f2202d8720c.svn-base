#include <velib/types/variant.h>
#include <velib/io/ve_vstream.h>

/// Gets the bit content of the variant from the application.
void veVariantFromBitBuffer(VeVariant *variant, void* buffer, un8 bitOffset)
{
	VeStream const* tmpStream;
	VeStream bitStream;

	// In order to save some code space, the memory is used directly as input
	// stream. The existing stream is restored when the values is obtained.
	tmpStream = veInStreamInit(&bitStream, buffer, 4);

	veInMoveBit(bitOffset);

	veInVariantLe(variant);

	veInRestore(tmpStream);
}

/// Copies the bit content of the variant to the application.
void veVariantToBitBuffer(VeVariant *variant, void* buffer, un8 bitOffset)
{
	VeStream const* tmpStream;
	VeStream bitStream;

	tmpStream = veOutStreamInit(&bitStream, buffer, 4);

	// write the value
	veOutMoveBit(bitOffset);

	veOutVariantLe(variant);

	veOutRestore(tmpStream);
}
