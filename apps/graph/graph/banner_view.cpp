#include "banner_view.h"
#include <assert.h>
#include <apps/i18n.h>

namespace Graph {

BannerView::BannerView() :
  Shared::XYBannerView(),
  m_derivativeView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_tangentEquationView(KDFont::SmallFont, I18n::Message::LinearRegressionFormula, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_aView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_bView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_numberOfSubviews(Shared::XYBannerView::k_numberOfSubviews)
{
}

View * BannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  if (index < Shared::XYBannerView::k_numberOfSubviews) {
    return Shared::XYBannerView::subviewAtIndex(index);
  }
  View * subviews[] = {&m_derivativeView, &m_tangentEquationView, &m_aView, &m_bView};
  return subviews[index - Shared::XYBannerView::k_numberOfSubviews];
}

}
