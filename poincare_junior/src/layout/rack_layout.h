#ifndef POINCARE_JUNIOR_RACK_LAYOUT_H
#define POINCARE_JUNIOR_RACK_LAYOUT_H

#include <kandinsky/context.h>

#include "../memory/edition_reference.h"
#include "rack.h"

namespace PoincareJ {

class LayoutCursor;
class Rack;

class RackLayout {
 public:
  static bool IsEmpty(const Tree* node) {
    assert(node->isRackLayout());
    return node->numberOfChildren() == 0;
  }
  static bool IsTrivial(const Rack* node) {
    return node->numberOfChildren() == 1 &&
           !node->child(0)->isVerticalOffsetLayout();
  }
  static KDSize Size(const Rack* node);
  static KDCoordinate Baseline(const Rack* node);
  static KDPoint ChildPosition(const Rack* node, int i);
  using Callback = void(const LayoutT* child, KDSize childSize,
                        KDCoordinate childBaseline, KDPoint position,
                        void* context);
  static void IterBetweenIndexes(const Rack* node, int leftPosition,
                                 int rightPosition, Callback callback,
                                 void* context);
  static KDSize SizeBetweenIndexes(const Rack* node, int leftPosition,
                                   int rightPosition);
  static KDCoordinate BaselineBetweenIndexes(const Rack* node, int leftPosition,
                                             int rightPosition);
  static bool ShouldDrawEmptyRectangle(const Rack* node);
  static bool ShouldDrawEmptyBaseAt(const Rack* node, int childIndex);
  static void RenderNode(const Rack* node, KDContext* ctx, KDPoint p,
                         bool isGridPlaceholder = false);

  static const LayoutCursor* s_layoutCursor;
};

}  // namespace PoincareJ

#endif
