#include "histogram_main_view.h"

namespace Statistics {

HistogramMainView::HistogramMainView(Escher::SelectableListView* listView)
    : m_displayBanner(false), m_listView(listView) {}

Escher::View* HistogramMainView::subviewAtIndex(int index) {
  if (index == 0) {
    return m_listView;
  }
  assert(index == 1);
  return &m_bannerView;
}

void HistogramMainView::layoutSubviews(bool force) {
  KDSize bannerSize = m_displayBanner
                          ? m_bannerView.minimalSizeForOptimalDisplay()
                          : KDSizeZero;
  setChildFrame(&m_bannerView,
                KDRect(0, bounds().height() - bannerSize.height(),
                       bannerSize.width(), bannerSize.height()),
                force);
  m_bannerView.reload();

  KDSize listSize(bounds().width(), bounds().height() - bannerSize.height());
  setChildFrame(m_listView, KDRect(KDPointZero, listSize), force);
}

}  // namespace Statistics
