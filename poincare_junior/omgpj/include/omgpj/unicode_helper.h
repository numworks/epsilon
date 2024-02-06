#ifndef UTILS_UNICODE_HELPER_H
#define UTILS_UNICODE_HELPER_H

#include <ion/unicode/utf8_decoder.h>

namespace PoincareJ {
class CPL;
}

namespace OMG {

size_t CodePointSearch(UnicodeDecoder* decoder, CodePoint c);
int CompareDecoders(UnicodeDecoder* a, UnicodeDecoder* b);
int CompareDecoderWithNullTerminatedString(UnicodeDecoder* decoder,
                                           const char* string);
const PoincareJ::CPL* CodePointLSearch(const PoincareJ::CPL* s, CodePoint c,
                                       const PoincareJ::CPL* stop);

}  // namespace OMG

#endif
