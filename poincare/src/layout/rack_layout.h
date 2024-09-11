#ifndef POINCARE_LAYOUT_RACK_LAYOUT_H
#define POINCARE_LAYOUT_RACK_LAYOUT_H

#include <kandinsky/context.h>
#include <poincare/layout_style.h>

#include "../memory/tree_ref.h"
#include "rack.h"

namespace Poincare::Internal {

class LayoutCursor;
class Rack;

class RackLayout {
 public:
  static bool IsEmpty(const Tree* l) {
    assert(l->isRackLayout());
    return l->numberOfChildren() == 0;
  }
  static bool IsTrivial(const Rack* l) {
    return l->numberOfChildren() == 1 && !l->child(0)->isVerticalOffsetLayout();
  }
  static KDSize Size(const Rack* l, bool showEmpty);
  static KDCoordinate Baseline(const Rack* l);
  static KDPoint ChildPosition(const Rack* l, int i);
  using Callback = void(const Layout* child, KDSize childSize,
                        KDCoordinate childBaseline, KDPoint position,
                        void* context);
  static void IterBetweenIndexes(const Rack* l, int leftPosition,
                                 int rightPosition, Callback callback,
                                 void* context, bool showEmpty);
  static KDSize SizeBetweenIndexes(const Rack* l, int leftPosition,
                                   int rightPosition, bool showEmpty = false);
  static KDCoordinate BaselineBetweenIndexes(const Rack* l, int leftPosition,
                                             int rightPosition);
  static bool ShouldDrawEmptyRectangle(const Rack* l);
  static bool ShouldDrawEmptyBaseAt(const Rack* l, int childIndex);
  static void RenderNode(const Rack* l, KDContext* ctx, KDPoint p,
                         bool showEmpty, bool isGridPlaceholder,
                         const LayoutStyle& style);
  // Find the base of a vertical offset or nullptr if an empty square is needed
  static const Layout* FindBase(const Rack* rack, const Layout* verticalOffset,
                                int verticalOffsetIndex);

  static const LayoutCursor* s_layoutCursor;
};

}  // namespace Poincare::Internal

#endif
