#include "product_layout.h"
#include "char_layout.h"
#include "horizontal_layout.h"

namespace Poincare {

ExpressionLayout * ProductLayout::clone() const {
  ProductLayout * layout = new ProductLayout(const_cast<ProductLayout *>(this)->argumentLayout(), const_cast<ProductLayout *>(this)->lowerBoundLayout(), const_cast<ProductLayout *>(this)->upperBoundLayout(), true);
  return layout;
}

int ProductLayout::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  return SequenceLayout::writeDerivedClassInBuffer("product", buffer, bufferSize, numberOfSignificantDigits);
}

void ProductLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Compute sizes.
  KDSize upperBoundSize = upperBoundLayout()->size();
  KDSize lowerBoundSizeWithNEquals = HorizontalLayout(new CharLayout('n'), new CharLayout('='), lowerBoundLayout()->clone(), false).size();

  // Render the Product symbol.
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSizeWithNEquals.width()-k_symbolWidth)/2),
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
    k_lineThickness, k_symbolHeight), expressionColor);
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSizeWithNEquals.width()-k_symbolWidth)/2),
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
    k_symbolWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSizeWithNEquals.width()-k_symbolWidth)/2)+k_symbolWidth,
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
    k_lineThickness, k_symbolHeight), expressionColor);

  // Render the "n=" and the parentheses.
  SequenceLayout::render(ctx, p, expressionColor, backgroundColor);
}

}
