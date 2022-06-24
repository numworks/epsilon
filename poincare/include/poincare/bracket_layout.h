#ifndef POINCARE_BRACKET_LAYOUT_NODE_H
#define POINCARE_BRACKET_LAYOUT_NODE_H

#include <escher/metric.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class BracketLayoutNode : public LayoutNode {
public:
  static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight) {
    return (ChildHeightDictatesHeight(childHeight) ? childHeight : k_minimalChildHeight) + k_verticalMargin * 2;
  }
  static KDCoordinate BaselineGivenChildHeightAndBaseline(KDCoordinate childHeight, KDCoordinate childBaseline) {
    return childBaseline + k_verticalMargin + (ChildHeightDictatesHeight(childHeight) ? 0 : (k_minimalChildHeight - childHeight) / 2);
  }

  BracketLayoutNode() :
    LayoutNode(),
    m_childHeightComputed(false),
    m_childHeight(0)
  {}

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void invalidAllSizesPositionsAndBaselines() override;
  bool isCollapsable(int * numberOfOpenBrackets, bool goingLeft) const override;

  // TreeNode
  size_t size() const override { return sizeof(BracketLayoutNode); }
  int numberOfChildren() const override { return 0; }
#if TREE_LOG
  const char * description() const override { return "BracketLayout"; }
#endif

protected:
  // The vertical margin is external in parentheses and internal in curly braces
  constexpr static KDCoordinate k_verticalMargin = 2;
  constexpr static KDCoordinate k_lineThickness = 1;

  static bool IsLeftBracket(Type t);
  static bool IsRightBracket(Type t);
  static KDPoint PositionGivenChildHeightAndBaseline(bool left, KDCoordinate bracketWidth, KDSize childSize, KDCoordinate childBaseline) {
    return KDPoint(
      left ? -bracketWidth : childSize.width(),
      ChildHeightDictatesHeight(childSize.height()) ? -k_verticalMargin : childBaseline - HeightGivenChildHeight(childSize.height()) / 2
    );
  }
  static KDCoordinate OptimalChildHeightGivenLayoutHeight(KDCoordinate layoutHeight) { return layoutHeight - k_verticalMargin * 2; }

private:
  // Minimal height at which the children dictates bracket height
  constexpr static KDCoordinate k_minimalChildHeight = Escher::Metric::MinimalBracketAndParenthesisChildHeight;

  static bool ChildHeightDictatesHeight(KDCoordinate childHeight) { return childHeight >= k_minimalChildHeight; }

  // LayoutNode
  KDCoordinate computeBaseline(const KDFont * font) override;
  KDPoint positionOfChild(LayoutNode * child, const KDFont * font) override;
  KDCoordinate childHeight(const KDFont * font);
  KDCoordinate computeChildHeight(const KDFont * font);
  KDSize computeSize(const KDFont * font) override { return KDSize(width(), HeightGivenChildHeight(childHeight(font))); }
  virtual KDCoordinate width() const = 0;

  bool m_childHeightComputed;
  uint16_t m_childHeight;
};

}

#endif
