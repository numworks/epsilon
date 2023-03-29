#include "histogram_banner_view.h"

#include <apps/constant.h>
#include <assert.h>

using namespace Escher;

namespace Statistics {

HistogramBannerView::HistogramBannerView()
    : m_seriesName(k_bannerFieldFormat),
      m_intervalBuffer(k_bannerFieldFormat),
      m_frequencyBuffer(k_bannerFieldFormat),
      m_relativeFrequencyBuffer(k_bannerFieldFormat) {}

View* HistogramBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View* subviews[] = {&m_seriesName, &m_intervalBuffer, &m_frequencyBuffer,
                      &m_relativeFrequencyBuffer};
  return subviews[index];
}

}  // namespace Statistics
