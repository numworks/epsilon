#include "layout_span.h"

#include <omg/unicode_helper.h>

#include "layout_span_decoder.h"

namespace Poincare::Internal {

size_t CodePointSearch(LayoutSpan span, CodePoint c) {
  LayoutSpanDecoder decoder(span);
  return OMG::CodePointSearch(&decoder, c);
}

bool HasCodePoint(LayoutSpan span, CodePoint c) {
  return CodePointSearch(span, c) != span.size();
}

}  // namespace Poincare::Internal
