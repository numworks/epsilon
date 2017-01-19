#include "banner_view.h"

namespace Graph {

BannerView::BannerView() :
  m_abscissaView(KDText::FontSize::Small, 0.5f, 0.5f),
  m_functionView(KDText::FontSize::Small, 0.5f, 0.5f),
  m_derivativeView(KDText::FontSize::Small, 0.5f, 0.5f),
  m_displayDerivative(false)
{
}

void BannerView::setDisplayDerivative(bool displayDerivative) {
  m_displayDerivative = displayDerivative;
}

bool BannerView::displayDerivative() {
  return m_displayDerivative;
}

int BannerView::numberOfSubviews() const {
  if (m_displayDerivative) {
    return 3;
  }
  return 2;
}

TextView * BannerView::textViewAtIndex(int i) {
  TextView * textViews[3] = {&m_abscissaView, &m_functionView, &m_derivativeView};
  return textViews[i];
}

}
