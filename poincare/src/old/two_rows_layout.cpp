#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/serialization_helper.h>

#include <algorithm>

namespace Poincare {

int TwoRowsLayoutNode::indexAfterHorizontalCursorMove(
    OMG::HorizontalDirection direction, int currentIndex,
    bool* shouldRedrawLayout) {
  if (currentIndex == k_outsideIndex) {
    /* When coming from the left, go to the n layout.
     * When coming from the right, go to the k layout. */
    return direction.isRight() ? k_upperLayoutIndex : k_lowerLayoutIndex;
  }
  return k_outsideIndex;
}

int TwoRowsLayoutNode::indexAfterVerticalCursorMove(
    OMG::VerticalDirection direction, int currentIndex,
    PositionInLayout positionAtCurrentIndex, bool* shouldRedrawLayout) {
  if (currentIndex == k_lowerLayoutIndex && direction.isUp()) {
    return k_upperLayoutIndex;
  }
  if (currentIndex == k_upperLayoutIndex && direction.isDown()) {
    return k_lowerLayoutIndex;
  }
  return k_cantMoveIndex;
}

LayoutNode::DeletionMethod
TwoRowsLayoutNode::deletionMethodForCursorLeftOfChild(int childIndex) const {
  if (childIndex == k_upperLayoutIndex && lowerLayout()->isEmpty()) {
    return DeletionMethod::DeleteParent;
  }
  if (childIndex == k_lowerLayoutIndex) {
    return DeletionMethod::TwoRowsLayoutMoveFromLowertoUpper;
  }
  return DeletionMethod::MoveLeft;
}

}  // namespace Poincare
