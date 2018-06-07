#include "banner_view.h"

namespace Regression {

BannerView::BannerView() :
  m_dotNameView(KDText::FontSize::Small, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_xView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_yView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_regressionTypeView(KDText::FontSize::Small, (I18n::Message)0, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_subText1(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_subText2(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_subText3(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_subText4(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_subText5(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

int BannerView::numberOfSubviews() const {
  return k_numberOfTextViews;
}

TextView * BannerView::textViewAtIndex(int i) const {
  const TextView * textViews[k_numberOfTextViews] = {&m_dotNameView, &m_xView, &m_yView, &m_regressionTypeView, &m_subText1, &m_subText2, &m_subText3, &m_subText4, &m_subText5};
  return (TextView *)textViews[i];
}

MessageTextView * BannerView::messageTextViewAtIndex(int i) const {
  if (i == 3) {
    return (MessageTextView *)&m_regressionTypeView;
  }
  return nullptr;
}

}
