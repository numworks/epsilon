#include "banner_view.h"

namespace Regression {

BannerView::BannerView() :
  m_regressionTypeView(KDText::FontSize::Small, (I18n::Message)0, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_slopeView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_yInterceptView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_xView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_yView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

int BannerView::numberOfSubviews() const {
  return 5;
}

TextView * BannerView::textViewAtIndex(int i) const {
  const TextView * textViews[5] = {&m_regressionTypeView, &m_slopeView, &m_yInterceptView, &m_xView, &m_yView};
  return (TextView *)textViews[i];
}

MessageTextView * BannerView::messageTextViewAtIndex(int i) const {
  if (i == 0) {
    return (MessageTextView *)&m_regressionTypeView;
  }
  return nullptr;
}

}
