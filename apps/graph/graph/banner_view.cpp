#include "banner_view.h"
#include <apps/i18n.h>

namespace Graph {

BannerView::BannerView() :
  m_abscissaView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_functionView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_derivativeView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_tangentEquationView(KDFont::SmallFont, I18n::Message::LinearRegressionFormula, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_aView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_bView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_numberOfSubviews(2)
{
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
