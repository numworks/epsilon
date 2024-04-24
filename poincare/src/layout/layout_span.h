#ifndef POINCARE_LAYOUT_LAYOUT_SPAN_H
#define POINCARE_LAYOUT_LAYOUT_SPAN_H

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

}  // namespace Poincare::Internal
#endif
