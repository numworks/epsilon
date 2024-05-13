#include "layout_span.h"

#include <omg/unicode_helper.h>

#include "layout_span_decoder.h"

namespace Poincare::Internal {

size_t CodePointSearch(Poincare::Internal::LayoutSpan span, CodePoint c) {
  Poincare::Internal::LayoutSpanDecoder decoder(span);
  return OMG::CodePointSearch(&decoder, c);
}

}  // namespace Poincare::Internal
