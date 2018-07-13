#include <poincare/product_layout_node.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>

namespace Poincare {

int ProductLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  return SequenceLayoutNode::writeDerivedClassInBuffer("product", buffer, bufferSize, numberOfSignificantDigits);
}

void ProductLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Compute sizes.
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDSize lowerBoundSizeWithNEquals = HorizontalLayoutRef(CharLayoutRef('n'), CharLayoutRef('='), LayoutRef(lowerBoundLayout()).clone()).layoutSize();

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
  SequenceLayoutNode::render(ctx, p, expressionColor, backgroundColor);
}

}
