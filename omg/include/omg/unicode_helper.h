#ifndef OMG_UNICODE_HELPER_H
#define OMG_UNICODE_HELPER_H

#include <ion/unicode/utf8_decoder.h>

namespace Poincare::Internal {
class CPL;
class Tree;
}  // namespace Poincare::Internal

namespace OMG {

size_t CodePointSearch(UnicodeDecoder* decoder, CodePoint c);

inline size_t CodePointSearch(const char* string, CodePoint c);
inline size_t CodePointSearch(const Poincare::Internal::Tree* first, int length,
                              CodePoint c);

int CompareDecoders(UnicodeDecoder* a, UnicodeDecoder* b);
int CompareDecoderWithNullTerminatedString(UnicodeDecoder* decoder,
                                           const char* string);
int CompareCPLWithNullTerminatedString(const Poincare::Internal::CPL* s,
                                       int length, const char* string);
const Poincare::Internal::CPL* CodePointLSearch(
    const Poincare::Internal::CPL* s, CodePoint c,
    const Poincare::Internal::CPL* stop);

}  // namespace OMG

#endif
