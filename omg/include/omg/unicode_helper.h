#ifndef OMG_UNICODE_HELPER_H
#define OMG_UNICODE_HELPER_H

#include <omg/utf8_decoder.h>
#include <poincare/src/layout/layout_span.h>

namespace Poincare::Internal {
class CPL;
class Tree;
}  // namespace Poincare::Internal

namespace OMG {

size_t CodePointSearch(ForwardUnicodeDecoder* decoder, CodePoint c);

size_t CodePointSearch(const char* string, CodePoint c);
size_t CodePointSearch(Poincare::Internal::LayoutSpan span, CodePoint c);

int CompareDecoders(ForwardUnicodeDecoder* a, ForwardUnicodeDecoder* b);
int CompareDecoderWithNullTerminatedString(UnicodeDecoder* decoder,
                                           const char* string);

}  // namespace OMG

#endif
