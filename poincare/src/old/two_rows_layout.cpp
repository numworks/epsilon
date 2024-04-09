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

KDSize TwoRowsLayoutNode::computeSize(KDFont::Size font) {
  return KDSize(rowsWidth(font) + 2 * k_parenthesisWidth,
                rowsHeight(font) + upperMargin(font) + lowerMargin(font));
}

KDCoordinate TwoRowsLayoutNode::computeBaseline(KDFont::Size font) {
  return (rowsHeight(font) + 1) / 2;
}

KDPoint TwoRowsLayoutNode::positionOfChild(LayoutNode* child,
                                           KDFont::Size font) {
  KDSize size = computeSize(font);
  KDCoordinate horizontalCenter = size.width() / 2;
  if (child == upperLayout()) {
    return KDPoint(
        horizontalCenter - upperLayout()->layoutSize(font).width() / 2,
        upperMargin(font));
  }
  assert(child == lowerLayout());
  return KDPoint(horizontalCenter - lowerLayout()->layoutSize(font).width() / 2,
                 size.height() - lowerLayout()->layoutSize(font).height() -
                     lowerMargin(font));
}

}  // namespace Poincare
