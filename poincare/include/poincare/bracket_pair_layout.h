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
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;

protected:
  // Minimal height at which the children dictates bracket height
  constexpr static KDCoordinate k_minimalChildHeight = Escher::Metric::MinimalBracketAndParenthesisChildHeight;

  static bool ChildHeightDictatesHeight(KDCoordinate childHeight) { return childHeight >= k_minimalChildHeight; }
  static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight, KDCoordinate verticalMargin) {
    return (ChildHeightDictatesHeight(childHeight) ? childHeight : k_minimalChildHeight) + verticalMargin * 2;
  }
  static KDCoordinate BaselineGivenChildHeightAndBaseline(KDCoordinate childHeight, KDCoordinate childBaseline, KDCoordinate verticalMargin) {
    return childBaseline + verticalMargin + (ChildHeightDictatesHeight(childHeight) ? 0 : (k_minimalChildHeight - childHeight) / 2);
  }
  static KDPoint ChildOffset(KDCoordinate verticalMargin, KDCoordinate bracketWidth) { return KDPoint(bracketWidth, verticalMargin); }
  static KDPoint PositionGivenChildHeightAndBaseline(bool left, KDCoordinate bracketWidth, KDSize childSize, KDCoordinate childBaseline, KDCoordinate verticalMargin) {
    return KDPoint(
      left ? -bracketWidth : childSize.width(),
      ChildHeightDictatesHeight(childSize.height()) ? -verticalMargin : childBaseline - HeightGivenChildHeight(childSize.height(), verticalMargin) / 2
    );
  }
  static KDCoordinate OptimalChildHeightGivenLayoutHeight(KDCoordinate layoutHeight, KDCoordinate verticalMargin) { return layoutHeight - verticalMargin * 2; }

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override;
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override;

  LayoutNode * childLayout() const { return childAtIndex(0); }
  int serializeWithSymbol(char symbolOpen, char symbolClose, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
  virtual KDCoordinate bracketWidth() const = 0;
  virtual KDCoordinate verticalMargin() const = 0;
  virtual void renderOneBracket(bool left, KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) = 0;
};

}

#endif
