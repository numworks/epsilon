#include "banner_view.h"
#include <assert.h>

namespace Periodic {

// BannerView::DotView

void BannerView::DotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillAntialiasedCircle(KDPointZero, k_dotDiameter / 2, m_color, k_backgroundColor);
}

void BannerView::DotView::setColor(KDColor color) {
  if (color != m_color) {
    markRectAsDirty(bounds());
    m_color = color;
  }
}

// BannerView

void BannerView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_borderHeight), Palette::SystemGrayDark);
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
  KDCoordinate x = k_dotLeftMargin;

  AtomicNumber z = m_dataSource->selectedElement();
  if (z != ElementsViewDataSource::k_noElement) {
    KDRect dotRect = KDRect(x, (bounds().height() - k_dotDiameter) / 2, k_dotDiameter, k_dotDiameter);
    m_dotView.setFrame(dotRect, force);
    m_dotView.setColor(m_dataSource->coloring()->colorPairForElement(z).fg());
    x += dotRect.width() + k_dotLegendMargin;
  } else {
    m_dotView.setFrame(KDRectZero, force);
  }

  m_textView.setFrame(KDRect(x, k_borderHeight, bounds().width() - x, k_bannerHeight), force);
  char buffer[Escher::BufferTextView::k_maxNumberOfChar];
  m_dataSource->coloring()->setLegendForElement(z, buffer, Escher::BufferTextView::k_maxNumberOfChar);
  m_textView.setText(buffer);
}

}
