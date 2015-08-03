#ifndef _VELIB_IO_VE_VSTREAM_H_
#define _VELIB_IO_VE_VSTREAM_H_

/**
 * @defgroup VE_IO_VSTREAM IO-stream variant
 * @{
 * @ingroup VE_IO
 * @brief
 * Adds variant support to the @ref VE_IO_STREAM which is useful for building
 * messages on the fly, either because they are user configurable, defined in
 * data structure or message structure is obtained over the bus.
 */

#include <velib/io/ve_stream.h>
#include <velib/types/variant.h>

VE_DCL void veInVariantBe(VeVariant* variant);
VE_DCL void veInVariantLe(VeVariant* variant);
VE_DCL void veOutVariantBe(VeVariant* variant);
VE_DCL void veOutVariantLe(VeVariant* variant);

/// @}

#endif
