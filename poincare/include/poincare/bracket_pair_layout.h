#ifndef POINCARE_BRACKET_PAIR_LAYOUT_H
#define POINCARE_BRACKET_PAIR_LAYOUT_H

#include <escher/metric.h>
#include <poincare/layout_node.h>

namespace Poincare {

class BracketPairLayoutNode : public LayoutNode {
 public:
  constexpr static KDCoordinate k_lineThickness = 1;
  // TreeNode
  size_t size() const override { return sizeof(BracketPairLayoutNode); }
  int numberOfChildren() const override { return 1; }

  // LayoutNode
  DeletionMethod deletionMethodForCursorLeftOfChild(
      int childIndex) const override;

 protected:
  static KDCoordinate realVerticalMargin(KDCoordinate childHeight,
                                         KDCoordinate verticalMargin) {
    // If the child height is bellow the threshold, make it bigger so that
    // The bracket pair maintains the right height

    // Minimal height at which the children dictates bracket height
    constexpr KDCoordinate k_minimalChildHeight =
        Escher::Metric::MinimalBracketAndParenthesisChildHeight;

    if (childHeight < k_minimalChildHeight)
      verticalMargin += (k_minimalChildHeight - childHeight) / 2;

    return verticalMargin;
  }

  static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight,
                                             KDCoordinate verticalMargin) {
    return childHeight + 2 * realVerticalMargin(childHeight, verticalMargin);
  }

  static KDCoordinate BaselineGivenChildHeightAndBaseline(
      KDCoordinate childHeight, KDCoordinate childBaseline,
      KDCoordinate verticalMargin) {
    return childBaseline + realVerticalMargin(childHeight, verticalMargin);
  }
  static KDPoint ChildOffset(KDCoordinate verticalMargin,
                             KDCoordinate bracketWidth,
                             KDCoordinate childHeight) {
    return KDPoint(bracketWidth,
                   realVerticalMargin(childHeight, verticalMargin));
  }
  static KDPoint PositionGivenChildHeight(bool left, KDCoordinate bracketWidth,
                                          KDSize childSize,
                                          KDCoordinate verticalMargin) {
    return KDPoint(left ? -bracketWidth : childSize.width(),
                   -realVerticalMargin(childSize.height(), verticalMargin));
  }
  static KDCoordinate OptimalChildHeightGivenLayoutHeight(
      KDCoordinate layoutHeight, KDCoordinate verticalMargin) {
    return layoutHeight - verticalMargin * 2;
  }

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode* child, KDFont::Size font) override;
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) override;

  LayoutNode* childLayout() const { return childAtIndex(0); }
  int serializeWithSymbol(char symbolOpen, char symbolClose, char* buffer,
                          int bufferSize,
                          Preferences::PrintFloatMode floatDisplayMode,
                          int numberOfSignificantDigits) const;
  virtual KDCoordinate bracketWidth() const = 0;
  virtual KDCoordinate verticalMargin() const = 0;
  virtual void renderOneBracket(bool left, KDContext* ctx, KDPoint p,
                                KDGlyph::Style style) = 0;
};

}  // namespace Poincare

#endif
