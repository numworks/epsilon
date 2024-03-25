#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <escher/palette.h>
#include <omg/directions.h>
#include <poincare/context.h>
#include <poincare/layout_node.h>
#include <poincare/tree_handle.h>
#include <poincare/trinary_boolean.h>

namespace PoincareJ {
class LayoutCursor;
}

namespace Poincare {

class OLayout : public TreeHandle {
 public:
  OLayout() : TreeHandle() {}
  OLayout(const LayoutNode *node) : TreeHandle(node) {}
  OLayout clone() const;
  LayoutNode *node() const {
    assert(isUninitialized() ||
           (TreeHandle::node() && !TreeHandle::node()->isGhost()));
    return static_cast<LayoutNode *>(TreeHandle::node());
  }

  bool isIdenticalTo(OLayout l, bool makeEditable = false) const {
    return isUninitialized() ? l.isUninitialized()
                             : node()->isIdenticalTo(l, makeEditable);
  }

  // Rendering
  void render(KDContext *ctx, KDPoint p, KDGlyph::Style style) const {
    return node()->render(ctx, p, style);
  }
  KDSize layoutSize(KDFont::Size font,
                    PoincareJ::LayoutCursor *cursor = nullptr) const;
  KDCoordinate baseline(KDFont::Size font,
                        PoincareJ::LayoutCursor *cursor = nullptr) const;
  void invalidAllSizesPositionsAndBaselines() {
    // TODO remember if cursor was in layout and hide this method
    return node()->invalidAllSizesPositionsAndBaselines();
  }

  // Serialization
  size_t serializeForParsing(char *buffer, size_t bufferSize) const {
    return node()->serialize(buffer, bufferSize);
  }
  size_t serializeParsedExpression(char *buffer, size_t bufferSize,
                                   Context *context) const;
};

}  // namespace Poincare

#endif

// TODO remove this hack
#include "junior_layout.h"
