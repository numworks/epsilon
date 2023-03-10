#include <escher/expression_view.h>
#include <escher/palette.h>
#include <poincare/code_point_layout.h>

#include <algorithm>

using namespace Poincare;

namespace Escher {

ExpressionView::ExpressionView(float horizontalAlignment,
                               float verticalAlignment, KDColor textColor,
                               KDColor backgroundColor, KDFont::Size font)
    : GlyphsView(font, horizontalAlignment, verticalAlignment, textColor,
                 backgroundColor),
      m_horizontalMargin(0) {}

bool ExpressionView::setLayout(Layout layoutR) {
  /* Check m_layout.wasErasedByException(), otherwise accessing m_layout would
   * result in an ACCESS ERROR. */
  bool shouldRedraw =
      m_layout.wasErasedByException() || !m_layout.isIdenticalTo(layoutR);
  /* We need to overwrite m_layout anyway so that identifiers and reference
   * counters are properly handled. */
  m_layout = layoutR;
  if (shouldRedraw) {
    markRectAsDirty(bounds());
  }
  return shouldRedraw;
}

int ExpressionView::numberOfLayouts() const {
  return m_layout.numberOfDescendants(true);
}

KDSize ExpressionView::minimalSizeForOptimalDisplay() const {
  if (m_layout.isUninitialized()) {
    return KDSizeZero;
  }
  KDSize expressionSize = m_layout.layoutSize(m_font);
  return KDSize(expressionSize.width() + 2 * m_horizontalMargin,
                expressionSize.height());
}

KDPoint ExpressionView::drawingOrigin() const {
  KDSize expressionSize = m_layout.layoutSize(m_font);
  return KDPoint(
      m_horizontalMargin +
          m_horizontalAlignment * (bounds().width() - 2 * m_horizontalMargin -
                                   expressionSize.width()),
      std::max<KDCoordinate>(
          0,
          m_verticalAlignment * (bounds().height() - expressionSize.height())));
}

void ExpressionView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);
  if (!m_layout.isUninitialized()) {
    m_layout.draw(ctx, drawingOrigin(), m_font, m_textColor, m_backgroundColor,
                  selection());
  }
}

}  // namespace Escher
