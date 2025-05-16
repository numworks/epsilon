#ifndef POINCARE_LAYOUT_LAYOUT_SPAN_H
#define POINCARE_LAYOUT_LAYOUT_SPAN_H

#include <omg/code_point.h>

#include <span>

#include "rack.h"

namespace Poincare::Internal {

using LayoutSpan = std::span<const Layout>;

inline LayoutSpan MakeSpan(const Rack* rack) {
  return LayoutSpan(rack->child(0), rack->numberOfChildren());
}

size_t CodePointSearch(LayoutSpan span, CodePoint c);

// Returns true if the text had the code point
bool HasCodePoint(LayoutSpan span, CodePoint c);

}  // namespace Poincare::Internal
#endif
