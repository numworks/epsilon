#include "product_layout.h"
#include <string.h>
#include <assert.h>

namespace Poincare {

ExpressionLayout * ProductLayout::clone() const {
  ProductLayout * layout = new ProductLayout(const_cast<ProductLayout *>(this)->argumentLayout(), const_cast<ProductLayout *>(this)->lowerBoundLayout(), const_cast<ProductLayout *>(this)->upperBoundLayout(), true);
  return layout;
}

int ProductLayout::writeTextInBuffer(char * buffer, int bufferSize) const {
  return SequenceLayout::writeDerivedClassInBuffer("product", buffer, bufferSize);
}

void ProductLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize upperBoundSize = upperBoundLayout()->size();
  KDSize lowerBoundSize = lowerBoundLayout()->size();
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSize.width()-k_symbolWidth)/2),
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
    k_lineThickness, k_symbolHeight), expressionColor);
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSize.width()-k_symbolWidth)/2),
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
    k_symbolWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSize.width()-k_symbolWidth)/2)+k_symbolWidth,
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
    k_lineThickness, k_symbolHeight), expressionColor);
}

}
