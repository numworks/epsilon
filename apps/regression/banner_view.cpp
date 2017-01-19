#include "banner_view.h"

namespace Regression {

BannerView::BannerView() :
  m_regressionTypeView(KDText::FontSize::Small, nullptr, 0.5f, 0.5f),
  m_slopeView(KDText::FontSize::Small, 0.5f, 0.5f),
  m_yInterceptView(KDText::FontSize::Small, 0.5f, 0.5f),
  m_xView(KDText::FontSize::Small, 0.5f, 0.5f),
  m_yView(KDText::FontSize::Small, 0.5f, 0.5f)
{
}

int BannerView::numberOfSubviews() const {
  return 5;
}

TextView * BannerView::textViewAtIndex(int i) {
  TextView * textViews[5] = {&m_regressionTypeView, &m_slopeView, &m_yInterceptView, &m_xView, &m_yView};
  return textViews[i];
}

}
