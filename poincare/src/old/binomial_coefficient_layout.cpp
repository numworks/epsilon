#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/binomial_coefficient.h>
#include <poincare/old/serialization_helper.h>

#include <algorithm>

namespace Poincare {

size_t BinomialCoefficientLayoutNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      BinomialCoefficient::s_functionHelper.aliasesList().mainAlias(),
      SerializationHelper::ParenthesisType::System);
}

void BinomialCoefficientLayoutNode::render(KDContext* ctx, KDPoint p,
                                           KDGlyph::Style style) {
  // Render the parentheses.
  KDCoordinate childHeight = rowsHeight(style.font);
  KDCoordinate rightParenthesisPointX =
      rowsWidth(style.font) + k_parenthesisWidth;
  ParenthesisLayoutNode::RenderWithChildHeight(
      true, childHeight, ctx, p, style.glyphColor, style.backgroundColor);
  ParenthesisLayoutNode::RenderWithChildHeight(
      false, childHeight, ctx,
      p.translatedBy(KDPoint(rightParenthesisPointX, 0)), style.glyphColor,
      style.backgroundColor);
}

}  // namespace Poincare
