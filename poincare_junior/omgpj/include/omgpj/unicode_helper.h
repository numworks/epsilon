#ifndef UTILS_UNICODE_HELPER_H
#define UTILS_UNICODE_HELPER_H

#include <ion/unicode/utf8_decoder.h>

namespace OMG {

size_t CodePointSearch(UnicodeDecoder * decoder, CodePoint c);
int CompareDecoders(UnicodeDecoder * a, UnicodeDecoder * b);
int CompareDecoderWithNullTerminatedString(UnicodeDecoder * decoder, const char * string);

}

#endif
