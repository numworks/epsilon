#include "kallax_info.h"

#ifndef HEADER_SECTION
#define HEADER_SECTION
#endif

#ifndef FORCE_LINK
#define FORCE_LINK
#endif

extern Kallax f;
constexpr KallaxInfo HEADER_SECTION FORCE_LINK kallax_infos((uint32_t)(&f));
