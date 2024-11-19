#include "histogram_main_view.h"

namespace Statistics {

HistogramMainView::HistogramMainView(Escher::SelectableListView* listView)
    : m_listView(listView) {}

Escher::View* HistogramMainView::subviewAtIndex(int index) {
  if (index == 0) {
    return m_listView;
  }
  assert(index == 1);
  return &m_bannerView;
}

void HistogramMainView::layoutSubviews(bool force) {
  KDSize bannerSize = m_bannerView.minimalSizeForOptimalDisplay();
  setChildFrame(&m_bannerView,
                KDRect(0, bounds().height() - bannerSize.height(),
                       bounds().width(), bannerSize.height()),
                force);
  setChildFrame(
      m_listView,
      KDRect(0, 0, bounds().width(), bounds().height() - bannerSize.height()),
      force);
}

}  // namespace Statistics
