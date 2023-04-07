#include "expression_view.h"

#include <algorithm>

using namespace PoincareJ;

namespace CalculationJunior {

bool AbstractExpressionView::setLayout(Layout layoutR) {
  if (!getLayout().treeIsIdenticalTo(layoutR)) {
    privateSetLayout(layoutR);
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

KDSize AbstractExpressionView::minimalSizeForOptimalDisplay() const {
  if (!getLayout().isInitialized()) {
    return KDSizeZero;
  }
  KDSize expressionSize = getLayout().size(m_glyphFormat.style.font);
  return KDSize(expressionSize.width() + 2 * m_horizontalMargin,
                expressionSize.height());
}

KDPoint AbstractExpressionView::drawingOrigin() const {
  KDSize expressionSize = getLayout().size(m_glyphFormat.style.font);
  return KDPoint(
      m_horizontalMargin + m_glyphFormat.horizontalAlignment *
                               (bounds().width() - 2 * m_horizontalMargin -
                                expressionSize.width()),
      std::max<KDCoordinate>(
          0, m_glyphFormat.verticalAlignment *
                 (bounds().height() - expressionSize.height())));
}

void AbstractExpressionView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(rect, m_glyphFormat.style.backgroundColor);
  if (getLayout().isInitialized()) {
    // TODO : Implement Selection here (use selection())
    getLayout().draw(ctx, drawingOrigin(), m_glyphFormat.style.font,
                     m_glyphFormat.style.glyphColor,
                     m_glyphFormat.style.backgroundColor);
  }
}

}  // namespace CalculationJunior
