#ifndef POINCARE_PARENTHESIS_LAYOUT_H
#define POINCARE_PARENTHESIS_LAYOUT_H

#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class ParenthesisLayoutNode : public AutocompletedBracketPairLayoutNode {
 public:
  constexpr static KDCoordinate k_widthMargin = 1;
  constexpr static KDCoordinate k_curveWidth = 5;
  constexpr static KDCoordinate k_curveHeight = 7;
  constexpr static KDCoordinate k_minVerticalMargin = 2;
  constexpr static KDCoordinate k_parenthesisWidth =
      2 * k_widthMargin + k_curveWidth;

  static void RenderWithChildHeight(bool left, KDCoordinate childHeight,
                                    KDContext* ctx, KDPoint p,
                                    KDColor expressionColor,
                                    KDColor backgroundColor);
  static KDCoordinate Height(KDCoordinate childHeight) {
    return BracketPairLayoutNode::Height(childHeight, k_minVerticalMargin);
  }
  static KDCoordinate Baseline(KDCoordinate childHeight,
                               KDCoordinate childBaseline) {
    return BracketPairLayoutNode::Baseline(childHeight, childBaseline,
                                           k_minVerticalMargin);
  }
  static KDCoordinate VerticalMargin(KDCoordinate childHeight) {
    return BracketPairLayoutNode::VerticalMargin(childHeight,
                                                 k_minVerticalMargin);
  }

  // LayoutNode
  Type otype() const override { return Type::ParenthesisLayout; }

  // PoolObject
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ParenthesisLayout";
  }
#endif
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override {
    return serializeWithSymbol('(', ')', buffer, bufferSize, floatDisplayMode,
                               numberOfSignificantDigits);
  }

 private:
  // BracketPairLayoutNode
  KDCoordinate bracketWidth() const override { return k_parenthesisWidth; }
  KDCoordinate minVerticalMargin() const override {
    return k_minVerticalMargin;
  }
  void renderOneBracket(bool left, KDContext* ctx, KDPoint p,
                        KDGlyph::Style style) override {
    RenderWithChildHeight(left, childLayout()->layoutSize(style.font).height(),
                          ctx, p,
                          bracketColor(left ? Side::Left : Side::Right,
                                       style.glyphColor, style.backgroundColor),
                          style.backgroundColor);
  }
};

class ParenthesisLayout final
    : public LayoutOneChild<ParenthesisLayout, ParenthesisLayoutNode> {
 public:
  ParenthesisLayout() = delete;
  static ParenthesisLayout Builder() {
    return Builder(HorizontalLayout::Builder());
  }
  static ParenthesisLayout Builder(OLayout l) {
    // Ensure parenthesis layout has always an horizontal child
    return LayoutOneChild<ParenthesisLayout, ParenthesisLayoutNode>::Builder(
        l.isHorizontal() ? l : HorizontalLayout::Builder(l));
  }
};

}  // namespace Poincare

#endif
