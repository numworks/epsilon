#include "normal_probability_view.h"
#include <poincare/normal_distribution.h>
#include <algorithm>
#include <assert.h>

namespace Statistics {

NormalProbabilityView::NormalProbabilityView(Store * store) :
    MultipleDataView(store),
    // No bannerView given to the curve view because the display is handled here
    m_curveView(&m_graphRange, &m_cursor, &m_cursorView, store) {
}

Escher::View * NormalProbabilityView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_curveView;
  }
  assert(index == 1);
  return &m_bannerView;
}

void NormalProbabilityView::reload() {
  int maxNumberOfPairs = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    maxNumberOfPairs = std::max(maxNumberOfPairs, m_store->numberOfPairsOfSeries(i));
  }
  float yBound = Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<float>(0.5f/maxNumberOfPairs, 0.0f, 1.0f);
  assert(yBound <= 0.0f);

  m_graphRange.calibrate(m_curveView.bounds().width(), m_curveView.bounds().height(), m_store->minValueForAllSeries(), m_store->maxValueForAllSeries(), yBound, -yBound);
  layoutSubviews();
  m_curveView.reload();
}

void NormalProbabilityView::layoutDataSubviews(bool force) {
  KDCoordinate bannerHeight = bannerView()->minimalSizeForOptimalDisplay().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight) + 1; // +1 to make sure that all pixel rows are drawn
  KDRect frame = KDRect(0, 0, bounds().width(), subviewHeight);
  m_curveView.setFrame(frame, force);
}

}
