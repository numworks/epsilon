#include "histogram_main_view.h"

namespace Statistics {

HistogramMainView::HistogramMainView(Escher::SelectableListView* listView)
    : m_displayBanner(false), m_listView(listView) {}

void HistogramMainView::setDisplayBanner(bool isVisible) {
  if (isVisible == m_displayBanner) {
    return;
  }
  m_displayBanner = isVisible;
  layoutSubviews();
}

Escher::View* HistogramMainView::subviewAtIndex(int index) {
  if (index == 0) {
    return m_listView;
  }
  assert(index == 1);
  return &m_bannerView;
}

KDSize HistogramMainView::bannerSize() const {
  return m_displayBanner ? m_bannerView.minimalSizeForOptimalDisplay()
                         : KDSizeZero;
}

void HistogramMainView::layoutSubviews(bool force) {
  layoutBanner(force);
  const KDSize listSize(bounds().width(),
                        bounds().height() - bannerSize().height());
  setChildFrame(m_listView, KDRect(KDPointZero, listSize), force);
}

void HistogramMainView::layoutBanner(bool force) {
  const KDSize size = bannerSize();
  setChildFrame(
      &m_bannerView,
      KDRect(0, bounds().height() - size.height(), size.width(), size.height()),
      force);
}

}  // namespace Statistics
