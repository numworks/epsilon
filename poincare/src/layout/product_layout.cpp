#include "product_layout.h"
#include <string.h>
#include <assert.h>

namespace Poincare {

void ProductLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize upperBoundSize = m_upperBoundLayout->size();
  KDSize lowerBoundSize = m_lowerBoundLayout->size();
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSize.width()-k_symbolWidth)/2),
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, m_argumentLayout->baseline()-(k_symbolHeight+1)/2),
    k_lineThickness, k_symbolHeight), expressionColor);
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSize.width()-k_symbolWidth)/2),
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, m_argumentLayout->baseline()-(k_symbolHeight+1)/2),
    k_symbolWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSize.width()-k_symbolWidth)/2)+k_symbolWidth,
    p.y() + max(upperBoundSize.height()+k_boundHeightMargin, m_argumentLayout->baseline()-(k_symbolHeight+1)/2),
    k_lineThickness, k_symbolHeight), expressionColor);
}

}
