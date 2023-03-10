#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/product_layout.h>

#include <algorithm>

namespace Poincare {

int ProductLayoutNode::serialize(char* buffer, int bufferSize,
                                 Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits) const {
  return SequenceLayoutNode::writeDerivedClassInBuffer(
      "product", buffer, bufferSize, floatDisplayMode,
      numberOfSignificantDigits);
}

void ProductLayoutNode::render(KDContext* ctx, KDPoint p,
                               KDGlyph::Style style) {
  KDFont::Size font = style.font;
  // Compute sizes.
  KDSize upperBoundSize = upperBoundLayout()->layoutSize(font);
  KDSize lowerBoundNEqualsSize = lowerBoundSizeWithVariableEquals(font);

  // Render the Product symbol.
  ctx->fillRect(
      KDRect(p.x() +
                 std::max(
                     {0, (upperBoundSize.width() - SymbolWidth(font)) / 2,
                      (lowerBoundNEqualsSize.width() - SymbolWidth(font)) / 2}),
             p.y() + std::max(upperBoundSize.height() +
                                  UpperBoundVerticalMargin(font),
                              argumentLayout()->baseline(font) -
                                  (SymbolHeight(font) + 1) / 2),
             k_lineThickness, SymbolHeight(font)),
      style.glyphColor);
  ctx->fillRect(
      KDRect(p.x() +
                 std::max(
                     {0, (upperBoundSize.width() - SymbolWidth(font)) / 2,
                      (lowerBoundNEqualsSize.width() - SymbolWidth(font)) / 2}),
             p.y() + std::max(upperBoundSize.height() +
                                  UpperBoundVerticalMargin(font),
                              argumentLayout()->baseline(font) -
                                  (SymbolHeight(font) + 1) / 2),
             SymbolWidth(font), k_lineThickness),
      style.glyphColor);
  ctx->fillRect(
      KDRect(p.x() +
                 std::max({0, (upperBoundSize.width() - SymbolWidth(font)) / 2,
                           (lowerBoundNEqualsSize.width() - SymbolWidth(font)) /
                               2}) +
                 SymbolWidth(font),
             p.y() + std::max(upperBoundSize.height() +
                                  UpperBoundVerticalMargin(font),
                              argumentLayout()->baseline(font) -
                                  (SymbolHeight(font) + 1) / 2),
             k_lineThickness, SymbolHeight(font)),
      style.glyphColor);

  // Render the "n=" and the parentheses.
  SequenceLayoutNode::render(ctx, p, style);
}

}  // namespace Poincare
