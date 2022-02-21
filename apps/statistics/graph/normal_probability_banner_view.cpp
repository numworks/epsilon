#include "normal_probability_banner_view.h"
#include <assert.h>

namespace Statistics {

NormalProbabilityBannerView::NormalProbabilityBannerView() :
  m_seriesName(Font(), KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
  m_value(Font(), KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
  m_normalProbability(Font(), KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()) {
}

Escher::View * NormalProbabilityBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  Escher::View * subviews[] = {&m_seriesName, &m_value, &m_normalProbability};
  return subviews[index];
}

}
