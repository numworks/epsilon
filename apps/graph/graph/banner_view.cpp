#include "banner_view.h"
#include "../../i18n.h"

namespace Graph {

BannerView::BannerView() :
  m_abscissaView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_functionView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_derivativeView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_tangentEquationView(KDText::FontSize::Small, I18n::Message::RegressionFormula, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_aView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_bView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_numberOfSubviews(2)
{
}

void BannerView::setNumberOfSubviews(int numberOfSubviews) {
  m_numberOfSubviews = numberOfSubviews;
}

int BannerView::numberOfSubviews() const {
  return m_numberOfSubviews;
}

TextView * BannerView::textViewAtIndex(int i) const {
  const TextView * textViews[6] = {&m_abscissaView, &m_functionView, &m_derivativeView, &m_tangentEquationView, &m_aView, &m_bView};
  return (TextView *)textViews[i];
}

MessageTextView * BannerView::messageTextViewAtIndex(int i) const {
  if (i == 3) {
    return (MessageTextView *)&m_tangentEquationView;
  }
  return nullptr;
}

}
