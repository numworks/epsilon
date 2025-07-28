#ifndef OMG_UNICODE_HELPER_H
#define OMG_UNICODE_HELPER_H

#include <omg/utf8_decoder.h>

namespace OMG {

size_t CodePointSearch(ForwardUnicodeDecoder* decoder, CodePoint c);

size_t CodePointSearch(const char* string, CodePoint c);

int CompareDecoders(ForwardUnicodeDecoder* a, ForwardUnicodeDecoder* b);
int CompareDecoderWithNullTerminatedString(UnicodeDecoder* decoder,
                                           const char* string);

}  // namespace OMG

#endif
