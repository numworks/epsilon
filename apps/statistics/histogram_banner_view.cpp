#include "histogram_banner_view.h"
#include "../constant.h"
#include <assert.h>
#include <poincare.h>

namespace Statistics {

HistogramBannerView::HistogramBannerView() :
  m_intervalView(0.5f, 0.5f),
  m_sizeView(0.5f, 0.5f),
  m_frequencyView(0.5f, 0.5f)
{
}

int HistogramBannerView::numberOfSubviews() const {
  return 3;
}

TextView * HistogramBannerView::textViewAtIndex(int i) {
  TextView * textViews[3] = {&m_intervalView, &m_sizeView, &m_frequencyView};
  return textViews[i];
}

}
