#include "banner_view.h"

namespace Regression {

BannerView::BannerView() :
  m_dotNameView(KDText::FontSize::Small, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_xView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_yView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_regressionTypeView(KDText::FontSize::Small, (I18n::Message)0, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_slopeView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_yInterceptView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_rView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_r2View(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

int BannerView::numberOfSubviews() const {
  return 8;
}

TextView * BannerView::textViewAtIndex(int i) const {
  const TextView * textViews[8] = {&m_dotNameView, &m_xView, &m_yView, &m_regressionTypeView, &m_slopeView, &m_yInterceptView, &m_rView, &m_r2View};
  return (TextView *)textViews[i];
}

MessageTextView * BannerView::messageTextViewAtIndex(int i) const {
  if (i == 3) {
    return (MessageTextView *)&m_regressionTypeView;
  }
  return nullptr;
}

}
