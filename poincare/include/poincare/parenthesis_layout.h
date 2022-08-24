#ifndef POINCARE_PARENTHESIS_LAYOUT_H
#define POINCARE_PARENTHESIS_LAYOUT_H

#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/empty_layout.h>

namespace Poincare {

class ParenthesisLayoutNode : public AutocompletedBracketPairLayoutNode {
public:
  constexpr static KDCoordinate k_widthMargin = 1;
  constexpr static KDCoordinate k_curveWidth = 5;
  constexpr static KDCoordinate k_curveHeight = 7;
  constexpr static KDCoordinate k_verticalMargin = 2;
  constexpr static KDCoordinate k_parenthesisWidth = 2 * k_widthMargin + k_curveWidth;

  static void RenderWithChildHeight(bool left, KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);
  static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight) { return BracketPairLayoutNode::HeightGivenChildHeight(childHeight, k_verticalMargin); }
  static KDCoordinate BaselineGivenChildHeightAndBaseline(KDCoordinate childHeight, KDCoordinate childBaseline) {
    return BracketPairLayoutNode::BaselineGivenChildHeightAndBaseline(childHeight, childBaseline, k_verticalMargin);
  }
  static KDPoint PositionGivenChildHeightAndBaseline(bool left, KDSize childSize, KDCoordinate childBaseline) {
    return BracketPairLayoutNode::PositionGivenChildHeightAndBaseline(left, k_parenthesisWidth, childSize, childBaseline, k_verticalMargin);
  }

  // LayoutNode
  Type type() const override { return Type::ParenthesisLayout; }

  // TreeNode
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ParenthesisLayout";
  }
#endif
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return serializeWithSymbol('(', ')', buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  }

private:
  // BracketPairLayoutNode
  KDCoordinate bracketWidth() const override { return k_parenthesisWidth; }
  KDCoordinate verticalMargin() const override { return k_verticalMargin; }
  void renderOneBracket(bool left, KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override {
    RenderWithChildHeight(left, childLayout()->layoutSize(font).height(), ctx, p, bracketColor(left ? Side::Left : Side::Right, expressionColor, backgroundColor), backgroundColor);
  }
};

class ParenthesisLayout final : public LayoutOneChild<ParenthesisLayout, ParenthesisLayoutNode> {
public:
  ParenthesisLayout() = delete;
  static ParenthesisLayout Builder() { return Builder(EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow, false)); }
  static ParenthesisLayout Builder(Layout l) { return LayoutOneChild<ParenthesisLayout, ParenthesisLayoutNode>::Builder(l); }
};

}

#endif
