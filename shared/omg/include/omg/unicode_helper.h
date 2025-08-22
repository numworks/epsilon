#pragma once

#include <omg/utf8_decoder.h>

namespace OMG {

size_t CodePointSearch(ForwardUnicodeDecoder* decoder, CodePoint c);

size_t CodePointSearch(const char* string, CodePoint c);

int CompareDecoders(ForwardUnicodeDecoder* a, ForwardUnicodeDecoder* b,
                    bool ignoreCombining = false);
int CompareDecoderWithNullTerminatedString(UnicodeDecoder* decoder,
                                           const char* string);

}  // namespace OMG
