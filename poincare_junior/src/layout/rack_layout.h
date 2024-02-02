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
  static bool IsTrivial(const Tree* node) {
    return node->numberOfChildren() == 1 &&
           !node->child(0)->isVerticalOffsetLayout();
  }
  static KDSize Size(const Tree* node);
  static KDCoordinate Baseline(const Tree* node);
  static KDPoint ChildPosition(const Tree* node, int i);
  using Callback = void(const Tree* child, KDSize childSize,
                        KDCoordinate childBaseline, KDPoint position,
                        void* context);
  static void IterBetweenIndexes(const Tree* node, int leftPosition,
                                 int rightPosition, Callback callback,
                                 void* context);
  static KDSize SizeBetweenIndexes(const Tree* node, int leftPosition,
                                   int rightPosition);
  static KDCoordinate BaselineBetweenIndexes(const Tree* node, int leftPosition,
                                             int rightPosition);
  static bool ShouldDrawEmptyRectangle(const Tree* node);
  static bool ShouldDrawEmptyBaseAt(const Tree* node, int childIndex);
  static void RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                         bool isGridPlaceholder = false);

  static const LayoutCursor* s_layoutCursor;
};

}  // namespace PoincareJ

#endif
