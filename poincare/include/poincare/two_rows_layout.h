#ifndef POINCARE_TWO_ROWS_LAYOUT_NODE_H
#define POINCARE_TWO_ROWS_LAYOUT_NODE_H

#include <poincare/grid_layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_helper.h>
#include <poincare/old_layout.h>
#include <poincare/parenthesis_layout.h>

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

  // TreeNode
  constexpr static int k_upperLayoutIndex = 0;
  constexpr static int k_lowerLayoutIndex = 1;
  int numberOfChildren() const override final { return 2; }

 protected:
  constexpr static KDCoordinate k_parenthesisWidth =
      ParenthesisLayoutNode::k_parenthesisWidth;

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode* child, KDFont::Size font) override;

  KDCoordinate rowsWidth(KDFont::Size font) {
    return std::max(upperLayout()->layoutSize(font).width(),
                    lowerLayout()->layoutSize(font).width());
  }
  KDCoordinate rowsHeight(KDFont::Size font) {
    return upperLayout()->layoutSize(font).height() + rowsSeparator() +
           lowerLayout()->layoutSize(font).height();
  }
  LayoutNode* upperLayout() const { return childAtIndex(k_upperLayoutIndex); }
  LayoutNode* lowerLayout() const { return childAtIndex(k_lowerLayoutIndex); }

  virtual KDCoordinate upperMargin(KDFont::Size font) { return 0; }
  virtual KDCoordinate lowerMargin(KDFont::Size font) { return 0; }
  virtual KDCoordinate rowsSeparator() {
    return GridLayoutNode::k_gridEntryMargin;
  }
};

}  // namespace Poincare

#endif
