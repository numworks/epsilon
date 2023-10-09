#include "layout_view.h"

#include <algorithm>

using namespace PoincareJ;

namespace CalculationJunior {

bool AbstractLayoutView::setLayout(Layout layoutR) {
  if (!getLayout().treeIsIdenticalTo(layoutR)) {
    privateSetLayout(layoutR);
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

KDSize AbstractLayoutView::minimalSizeForOptimalDisplay() const {
  if (getLayout().isUninitialized()) {
    return KDSizeZero;
  }
  KDSize layoutSize = getLayout().size(m_glyphFormat.style.font);
  return KDSize(layoutSize.width() + 2 * m_horizontalMargin,
                layoutSize.height());
}

KDPoint AbstractLayoutView::drawingOrigin() const {
  KDSize layoutSize = getLayout().size(m_glyphFormat.style.font);
  return KDPoint(
      m_horizontalMargin +
          m_glyphFormat.horizontalAlignment *
              (bounds().width() - 2 * m_horizontalMargin - layoutSize.width()),
      std::max<KDCoordinate>(0, m_glyphFormat.verticalAlignment *
                                    (bounds().height() - layoutSize.height())));
}

void AbstractLayoutView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(rect, m_glyphFormat.style.backgroundColor);
  if (!getLayout().isUninitialized()) {
    // TODO : Implement Selection here (use selection())
    getLayout().draw(ctx, drawingOrigin(), m_glyphFormat.style.font,
                     m_glyphFormat.style.glyphColor,
                     m_glyphFormat.style.backgroundColor);
  }
}

}  // namespace CalculationJunior
