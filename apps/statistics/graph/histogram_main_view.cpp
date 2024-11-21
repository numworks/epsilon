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

KDRect HistogramMainView::bannerFrame() const {
  KDSize bannerSize = m_bannerView.minimalSizeForOptimalDisplay();
  return KDRect(0, bounds().height() - bannerSize.height(), bounds().width(),
                bannerSize.height());
}

void HistogramMainView::layoutSubviews(bool force) {
  KDSize bannerSize = m_isBannerVisible
                          ? m_bannerView.minimalSizeForOptimalDisplay()
                          : KDSizeZero;
  KDSize listSize(bounds().width(), bounds().height() - bannerSize.height());
  setChildFrame(&m_bannerView,
                KDRect(0, bounds().height() - bannerSize.height(),
                       bannerSize.width(), bannerSize.height()),
                force);
  m_bannerView.reload();

  /* setChildFrame will automatically highlight the selected list cell. However
   * we want to preserve the highlight state of this cell. */
  if (!m_listView->selectedCell()) {
    setChildFrame(m_listView, KDRect(0, 0, listSize.width(), listSize.height()),
                  force);
    return;
  }
  bool isSelectedCellHighlighted = m_listView->selectedCell()->isHighlighted();
  setChildFrame(m_listView, KDRect(0, 0, listSize.width(), listSize.height()),
                force);
  m_listView->selectedCell()->setHighlighted(isSelectedCellHighlighted);
}

}  // namespace Statistics
