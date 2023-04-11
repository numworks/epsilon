#include "expression_view.h"

#include <algorithm>

using namespace PoincareJ;

namespace CalculationJunior {

bool ExpressionView::setLayout(Layout layoutR) {
  /* Check m_layout.wasErasedByException(), otherwise accessing m_layout would
   * result in an ACCESS ERROR. */
  bool shouldRedraw = !m_layout.treeIsIdenticalTo(layoutR);
  // Overwrite m_layout anyway
  m_layout = layoutR;
  if (shouldRedraw) {
    markRectAsDirty(bounds());
  }
  return shouldRedraw;
}

KDSize ExpressionView::minimalSizeForOptimalDisplay() const {
  if (!m_layout.isInitialized()) {
    return KDSizeZero;
  }
  KDSize expressionSize = m_layout.size(m_glyphFormat.style.font);
  return KDSize(expressionSize.width() + 2 * m_horizontalMargin,
                expressionSize.height());
}

KDPoint ExpressionView::drawingOrigin() const {
  KDSize expressionSize = m_layout.size(m_glyphFormat.style.font);
  return KDPoint(
      m_horizontalMargin + m_glyphFormat.horizontalAlignment *
                               (bounds().width() - 2 * m_horizontalMargin -
                                expressionSize.width()),
      std::max<KDCoordinate>(
          0, m_glyphFormat.verticalAlignment *
                 (bounds().height() - expressionSize.height())));
}

void ExpressionView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(rect, m_glyphFormat.style.backgroundColor);
  if (m_layout.isInitialized()) {
    // TODO : Implement Selection here (use selection())
    m_layout.draw(ctx, drawingOrigin(), m_glyphFormat.style.font,
                  m_glyphFormat.style.glyphColor,
                  m_glyphFormat.style.backgroundColor);
  }
}

}  // namespace CalculationJunior
