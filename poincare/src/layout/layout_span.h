#ifndef POINCARE_LAYOUT_LAYOUT_SPAN_H
#define POINCARE_LAYOUT_LAYOUT_SPAN_H

#include <omg/code_point.h>

#include "rack.h"

namespace Poincare::Internal {

/* Holds a pointer to a Layout and a length, to refer to consecutive layouts
 * inside a Rack. */
struct LayoutSpan {
  LayoutSpan(const Layout* start, uint16_t length)
      : start(start), length(length) {}
  LayoutSpan(const Rack* rack)
      : start(rack->child(0)), length(rack->numberOfChildren()) {}

  bool isEmpty() const { return length == 0; }

  const Layout* start;
  uint16_t length;
};

size_t CodePointSearch(LayoutSpan span, CodePoint c);

// Returns true if the text had the code point
bool HasCodePoint(LayoutSpan span, CodePoint c);

}  // namespace Poincare::Internal
#endif
