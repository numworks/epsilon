#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/point.h>
#include <poincare/old/serialization_helper.h>

#include <algorithm>

namespace Poincare {

size_t Point2DLayoutNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode,
                                     numberOfSignificantDigits,
                                     PointNode::k_prefix);
}

void Point2DLayoutNode::render(KDContext* ctx, KDPoint p,
                               KDGlyph::Style style) {
  KDCoordinate upperLayoutHeight =
      upperLayout()->layoutSize(style.font).height();
  KDCoordinate lowerLayoutHeight =
      lowerLayout()->layoutSize(style.font).height();
  KDCoordinate right = k_parenthesisWidth + rowsWidth(style.font);
  KDCoordinate bottom = upperLayoutHeight + k_rowsSeparator;
  // Draw left parenthesis
  ParenthesisLayoutNode::RenderWithChildHeight(
      true, upperLayoutHeight, ctx, p, style.glyphColor, style.backgroundColor);
  // Draw right parenthesis
  ParenthesisLayoutNode::RenderWithChildHeight(
      false, lowerLayoutHeight, ctx, p.translatedBy(KDPoint(right, bottom)),
      style.glyphColor, style.backgroundColor);
  // Draw comma
  KDCoordinate topMargin = upperMargin(style.font);
  KDCoordinate commaHeight = KDFont::GlyphHeight(style.font);
  assert(commaHeight <= upperLayoutHeight);
  KDCoordinate commaPositionY =
      topMargin + (upperLayoutHeight - commaHeight) / 2;
  ctx->drawString(",", p.translatedBy(KDPoint(right, commaPositionY)), style);
}

}  // namespace Poincare
