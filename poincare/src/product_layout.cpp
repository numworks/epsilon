#include <poincare/product_layout.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>

namespace Poincare {

static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

int ProductLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SequenceLayoutNode::writeDerivedClassInBuffer("product", buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
}

void ProductLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Compute sizes.
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDSize lowerBoundNEqualsSize = lowerBoundSizeWithVariableEquals();

  // Render the Product symbol.
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundNEqualsSize.width()-k_symbolWidth)/2),
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
    k_lineThickness, k_symbolHeight), expressionColor);
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundNEqualsSize.width()-k_symbolWidth)/2),
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
    k_symbolWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundNEqualsSize.width()-k_symbolWidth)/2)+k_symbolWidth,
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
    k_lineThickness, k_symbolHeight), expressionColor);

  // Render the "n=" and the parentheses.
  SequenceLayoutNode::render(ctx, p, expressionColor, backgroundColor);
}

}
