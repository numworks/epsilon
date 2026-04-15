#include <apps/global_preferences.h>
#include <escher/layout_style.h>
#include <escher/layout_view.h>
#include <escher/palette.h>
#include <poincare/layout.h>

#include <algorithm>

using namespace Poincare;

namespace Escher {

bool LayoutView::setLayout(const Layout& layoutR) {
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
  KDSize layoutSize = layout()->layoutSize(font(), cursor());
  return KDSize(layoutSize.width() + 2 * m_horizontalMargin,
                layoutSize.height());
}

KDPoint LayoutView::drawingOrigin() const {
  KDSize layoutSize = layout()->layoutSize(font(), cursor());
  return KDPoint(
      m_horizontalMargin +
          m_textFormat.horizontalAlignment *
              (bounds().width() - 2 * m_horizontalMargin - layoutSize.width()),
      std::max<KDCoordinate>(0, m_textFormat.verticalAlignment *
                                    (bounds().height() - layoutSize.height())));
}

void LayoutView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(rect, glyphFormat().style.backgroundColor);
  if (!layout().isUninitialized()) {
    layout().draw(ctx, drawingOrigin(),
                  ToPoincareLayoutStyle(DefaultLayoutStyle(m_textFormat.style)),
                  cursor());
  }
}

}  // namespace Escher
