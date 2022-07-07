#include "banner_view.h"
#include <assert.h>

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

Escher::View * BannerView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_textView;
  }
  assert(index == 1);
  return &m_dotView;
}

void BannerView::layoutSubviews(bool force) {
  AtomicNumber z = m_dataSource->selectedElement();
  m_dotView.setColor(m_dataSource->coloring()->colorPairForElement(z).fg());
  KDRect dotRect = KDRect(k_dotLeftMargin, (bounds().height() - k_dotDiameter) / 2, k_dotDiameter, k_dotDiameter);
  m_dotView.setFrame(dotRect, true);

  m_textView.setFrame(KDRect(dotRect.x() + dotRect.width() + k_dotLegendMargin, k_borderHeight, bounds().width() - dotRect.width(), k_bannerHeight), force);
  char buffer[Escher::BufferTextView::k_maxNumberOfChar];
  m_dataSource->coloring()->setLegendForElement(z, buffer, Escher::BufferTextView::k_maxNumberOfChar);
  m_textView.setText(buffer);

}

}
