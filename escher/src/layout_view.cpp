#include <escher/layout_view.h>
#include <escher/palette.h>
#include <poincare/code_point_layout.h>

#include <algorithm>

using namespace Poincare;

namespace Escher {

bool LayoutView::setLayout(Layout layoutR) {
  /* Check m_layout.wasErasedByException(), otherwise accessing m_layout would
   * result in an ACCESS ERROR. */
  bool shouldRedraw =
      m_layout.wasErasedByException() || !m_layout.isIdenticalTo(layoutR);
  /* We need to overwrite m_layout anyway so that identifiers and reference
   * counters are properly handled. */
  m_layout = layoutR;
  if (shouldRedraw) {
    markWholeFrameAsDirty();
  }
  return shouldRedraw;
}

int LayoutView::numberOfLayouts() const {
  return m_layout.numberOfDescendants(true);
}

KDSize LayoutView::minimalSizeForOptimalDisplay() const {
  if (layout().isUninitialized()) {
    return KDSizeZero;
  }
  KDSize layoutSize = layout().layoutSize(font());
  return KDSize(layoutSize.width() + 2 * m_horizontalMargin,
                layoutSize.height());
}

KDPoint LayoutView::drawingOrigin() const {
  KDSize layoutSize = layout().layoutSize(font());
  return KDPoint(
      m_horizontalMargin +
          m_glyphFormat.horizontalAlignment *
              (bounds().width() - 2 * m_horizontalMargin - layoutSize.width()),
      std::max<KDCoordinate>(0, m_glyphFormat.verticalAlignment *
                                    (bounds().height() - layoutSize.height())));
}

void LayoutView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(rect, m_glyphFormat.style.backgroundColor);
  if (!layout().isUninitialized()) {
    layout().draw(ctx, drawingOrigin(), m_glyphFormat.style, cursor());
  }
}

}  // namespace Escher
