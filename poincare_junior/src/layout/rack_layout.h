#ifndef POINCARE_JUNIOR_RACK_LAYOUT_H
#define POINCARE_JUNIOR_RACK_LAYOUT_H

#include <kandinsky/context.h>

#include "../memory/edition_reference.h"

namespace PoincareJ {

class LayoutCursor;

class RackLayout {
 public:
  static bool IsEmpty(const Tree* node) {
    assert(node->isRackLayout());
    return node->numberOfChildren() == 0;
  }
  static KDSize Size(const Tree* node);
  static KDCoordinate Baseline(const Tree* node);
  static KDCoordinate ChildBaseline(const Tree* node, int i);
  static KDCoordinate ChildYPosition(const Tree* node, int i);
  static KDSize SizeBetweenIndexes(const Tree* node, int leftPosition,
                                   int rightPosition);
  static KDCoordinate BaselineBetweenIndexes(const Tree* node, int leftPosition,
                                             int rightPosition);
  static bool ShouldDrawEmptyRectangle(const Tree* node);
  static void RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                         bool isGridPlaceholder = false);

  static const LayoutCursor* layoutCursor;

 private:
  static KDFont::Size font;
};

}  // namespace PoincareJ

#endif
