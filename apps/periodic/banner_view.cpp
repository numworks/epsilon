#include "banner_view.h"

namespace Periodic {

// BannerView::DotView

void BannerView::DotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillAntialiasedCircle(KDPointZero, k_dotDiameter / 2, m_color, k_backgroundColor);
}

// BannerView

void BannerView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_borderHeight), Palette::SystemGreyDark);
  ctx->fillRect(KDRect(0, k_borderHeight, bounds().width(), k_bannerHeight), k_backgroundColor);
}

void BannerView::layoutSubviews(bool force) {
  AtomicNumber z = m_dataSource->selectedElement();
  m_dotView.setColor(m_dataSource->colorPairForElement(z).fg());
  m_dotView.setFrame(KDRect(k_dotLeftMargin, (bounds().height() - k_dotDiameter) / 2, k_dotDiameter, k_dotDiameter), true);
}

}
