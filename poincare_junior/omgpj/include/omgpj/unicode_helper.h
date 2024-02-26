#ifndef UTILS_UNICODE_HELPER_H
#define UTILS_UNICODE_HELPER_H

#include <ion/unicode/utf8_decoder.h>

namespace PoincareJ {
class CPL;
class Tree;
}  // namespace PoincareJ

namespace OMG {

size_t CodePointSearch(UnicodeDecoder* decoder, CodePoint c);

inline size_t CodePointSearch(const char* string, CodePoint c);
inline size_t CodePointSearch(const PoincareJ::Tree* first, int length,
                              CodePoint c);

int CompareDecoders(UnicodeDecoder* a, UnicodeDecoder* b);
int CompareDecoderWithNullTerminatedString(UnicodeDecoder* decoder,
                                           const char* string);
int CompareCPLWithNullTerminatedString(const PoincareJ::CPL* s, int length,
                                       const char* string);
const PoincareJ::CPL* CodePointLSearch(const PoincareJ::CPL* s, CodePoint c,
                                       const PoincareJ::CPL* stop);

}  // namespace OMG

#endif
