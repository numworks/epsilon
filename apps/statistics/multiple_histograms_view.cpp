#include "multiple_histograms_view.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

MultipleHistogramsView::MultipleHistogramsView(HistogramController * controller, Store * store) :
  m_histogramView1(controller, store, 0, nullptr, Palette::Red),
  m_histogramView2(controller, store, 1, nullptr, Palette::Blue),
  m_histogramView3(controller, store, 2, nullptr, Palette::Green),
  // TODO Share colors with stats/store_controller
  m_bannerView(),
  m_okView(),
  m_displayBanner(false),
  m_store(store)
{
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    HistogramView * histView = histogramViewAtIndex(i);
    histView->setDisplayBannerView(false);
    histView->setDisplayLabels(false);
    histView->setForceOkDisplay(true);
  }
}

void MultipleHistogramsView::reload() {
  layoutSubviews();
  m_histogramView1.reload();
  m_histogramView2.reload();
  m_histogramView3.reload();
}

HistogramView *  MultipleHistogramsView::histogramViewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  HistogramView * views[] = {&m_histogramView1, &m_histogramView2, &m_histogramView3};
  return views[index];
}

int MultipleHistogramsView::seriesOfSubviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews() - 1);
  return static_cast<HistogramView *>(subviewAtIndex(index))->series();
}

int MultipleHistogramsView::indexOfSubviewAtSeries(int series) {
  int displayedSubviewIndex = 0;
  for (int i = 0; i < 3; i++) {
    if (!m_store->seriesIsEmpty(i)) {
     if (i == series) {
        return displayedSubviewIndex;
     }
     displayedSubviewIndex++;
    } else if (i == series) {
      return -1;
    }
  }
  assert(false);
  return -1;
}

void MultipleHistogramsView::selectHistogram(int index) {
  changeHistogramSelection(index, true);
}

void MultipleHistogramsView::deselectHistogram(int index) {
  changeHistogramSelection(index, false);
}

void MultipleHistogramsView::drawRect(KDContext * ctx, KDRect rect) const {
  if (!m_displayBanner) {
    ctx->fillRect(bannerFrame(), KDColorWhite);
  }
}

int MultipleHistogramsView::numberOfSubviews() const {
  int result = m_store->numberOfNonEmptySeries();
  assert(result <= Store::k_numberOfSeries);
  return result + 1; // +1 for the banner view
}

KDRect MultipleHistogramsView::bannerFrame() const {
  KDCoordinate bannerHeight = m_bannerView.minimalSizeForOptimalDisplay().height();
  KDRect frame = KDRect(0, bounds().height() - bannerHeight, bounds().width(), bannerHeight);
  return frame;
}

View * MultipleHistogramsView::subviewAtIndex(int index) {
  if (index == numberOfSubviews() -1) {
    return &m_bannerView;
  }
  int seriesIndex = 0;
  int nonEmptySeriesIndex = -1;
  while (seriesIndex < Store::k_numberOfSeries) {
    if (!m_store->seriesIsEmpty(seriesIndex)) {
      nonEmptySeriesIndex++;
      if (nonEmptySeriesIndex == index) {
        return histogramViewAtIndex(seriesIndex);
      }
    }
    seriesIndex++;
  }
  assert(false);
  return nullptr;
}

void MultipleHistogramsView::layoutSubviews() {
  int numberHistogramSubviews = m_store->numberOfNonEmptySeries();
  assert(numberHistogramSubviews > 0);
  KDCoordinate bannerHeight = bannerFrame().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight)/numberHistogramSubviews;
  int displayedSubviewIndex = 0;
  for (int i = 0; i < 3; i++) {
    if (!m_store->seriesIsEmpty(i)) {
      HistogramView * histView = histogramViewAtIndex(i);
      KDRect frame = KDRect(0, displayedSubviewIndex*subviewHeight, bounds().width(), subviewHeight);
      histView->setFrame(frame);
      displayedSubviewIndex++;
      histView->setOkView(displayedSubviewIndex == numberHistogramSubviews ? &m_okView : nullptr);
    }
  }
  if (m_displayBanner) {
    m_bannerView.setFrame(bannerFrame());
  } else {
    KDRect frame = KDRect(0, bounds().height() - bannerHeight, bounds().width(), 0);
    m_bannerView.setFrame(frame);
  }
}

void MultipleHistogramsView::changeHistogramSelection(int index, bool select) {
  histogramViewAtIndex(index)->selectMainView(select);
  histogramViewAtIndex(index)->setDisplayLabels(select);
}

}
