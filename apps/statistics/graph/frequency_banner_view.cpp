#include "frequency_banner_view.h"
#include <assert.h>

namespace Statistics {

FrequencyBannerView::FrequencyBannerView() :
  m_seriesName(Font(), KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
  m_value(Font(), KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
  m_frequency(Font(), KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()) {
}

Escher::View * FrequencyBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  Escher::View * subviews[] = {&m_seriesName, &m_value, &m_frequency};
  return subviews[index];
}

}
