#include "histogram_banner_view.h"

#include <apps/constant.h>
#include <assert.h>

using namespace Escher;

namespace Statistics {

HistogramBannerView::HistogramBannerView()
    : m_seriesName(k_font, KDContext::k_alignCenter, KDContext::k_alignCenter,
                   TextColor(), BackgroundColor()),
      m_intervalBuffer(k_font, KDContext::k_alignCenter,
                       KDContext::k_alignCenter, TextColor(),
                       BackgroundColor()),
      m_frequencyBuffer(k_font, KDContext::k_alignCenter,
                        KDContext::k_alignCenter, TextColor(),
                        BackgroundColor()),
      m_relativeFrequencyBuffer(k_font, KDContext::k_alignCenter,
                                KDContext::k_alignCenter, TextColor(),
                                BackgroundColor()) {}

View* HistogramBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View* subviews[] = {&m_seriesName, &m_intervalBuffer, &m_frequencyBuffer,
                      &m_relativeFrequencyBuffer};
  return subviews[index];
}

}  // namespace Statistics
