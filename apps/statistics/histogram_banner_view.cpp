#include "histogram_banner_view.h"
#include "../constant.h"
#include <assert.h>
#include <poincare.h>

namespace Statistics {

HistogramBannerView::HistogramBannerView() :
  m_intervalView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sizeView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_frequencyView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

int HistogramBannerView::numberOfSubviews() const {
  return 3;
}

TextView * HistogramBannerView::textViewAtIndex(int i) const {
  const TextView * textViews[3] = {&m_intervalView, &m_sizeView, &m_frequencyView};
  return (TextView *)textViews[i];
}

}
