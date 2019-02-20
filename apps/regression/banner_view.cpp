#include "banner_view.h"

namespace Regression {

constexpr KDColor BannerView::k_textColor;
constexpr KDColor BannerView::k_backgroundColor;

BannerView::BannerView() :
  m_dotNameView(k_font, 0.0f, 0.5f, k_textColor, k_backgroundColor),
  m_xView(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_yView(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_regressionTypeView(k_font, (I18n::Message)0, 0.0f, 0.5f, k_textColor,k_backgroundColor),
  m_subText1(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_subText2(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_subText3(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_subText4(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_subText5(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor)
{
}

TextView * BannerView::textViewAtIndex(int i) const {
  const TextView * textViews[k_numberOfSubviews] = {&m_dotNameView, &m_xView, &m_yView, &m_regressionTypeView, &m_subText1, &m_subText2, &m_subText3, &m_subText4, &m_subText5};
  return (TextView *)textViews[i];
}

MessageTextView * BannerView::messageTextViewAtIndex(int i) const {
  if (i == 3) {
    return (MessageTextView *)&m_regressionTypeView;
  }
  return nullptr;
}

}
