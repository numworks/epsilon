#include <escher/centering_view.h>

namespace Escher {

CenteringView::CenteringView(View * v, KDColor background) :
    m_contentView(v), m_backgroundColor(background) {
}

KDSize CenteringView::minimalSizeForOptimalDisplay() const {
  m_contentView->setFrame(m_frame, false);
  return m_contentView->minimalSizeForOptimalDisplay();
}

void CenteringView::layoutSubviews(bool force) {
  m_contentView->setFrame(m_frame, false);
  KDSize requiredSize = m_contentView->minimalSizeForOptimalDisplay();
  KDCoordinate dx = m_frame.width() - requiredSize.width();
  KDCoordinate dy = m_frame.height() - requiredSize.height();
  dx = zeroIfNegative(dx);
  dy = zeroIfNegative(dy);
  m_contentView->setFrame(KDRect(KDPoint(dx / 2, dy / 2), requiredSize), force);
}

void CenteringView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);
}

}  // namespace Escher
