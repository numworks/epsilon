#ifndef POINCARE_TWO_ROWS_LAYOUT_NODE_H
#define POINCARE_TWO_ROWS_LAYOUT_NODE_H

#include "grid_layout.h"
#include "layout_cursor.h"
#include "layout_helper.h"
#include "old_layout.h"
#include "parenthesis_layout.h"

namespace Poincare {

class TwoRowsLayoutNode : public LayoutNode {
 public:
  using LayoutNode::LayoutNode;

  int indexAfterHorizontalCursorMove(OMG::HorizontalDirection direction,
                                     int currentIndex,
                                     bool* shouldRedrawLayout) override;
  int indexAfterVerticalCursorMove(OMG::VerticalDirection direction,
                                   int currentIndex,
                                   PositionInLayout positionAtCurrentIndex,
                                   bool* shouldRedrawLayout) override;
  DeletionMethod deletionMethodForCursorLeftOfChild(
      int childIndex) const override;

  // PoolObject
  constexpr static int k_upperLayoutIndex = 0;
  constexpr static int k_lowerLayoutIndex = 1;
  int numberOfChildren() const override final { return 2; }

 protected:
  constexpr static KDCoordinate k_parenthesisWidth =
      ParenthesisLayoutNode::k_parenthesisWidth;
};

}  // namespace Poincare

#endif
