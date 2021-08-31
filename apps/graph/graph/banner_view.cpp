#include "banner_view.h"
#include <assert.h>
#include <apps/i18n.h>

using namespace Escher;

namespace Graph {

BannerView::BannerView(
  Responder * parentResponder,
  InputEventHandlerDelegate * inputEventHandlerDelegate,
  TextFieldDelegate * textFieldDelegate
) :
  Shared::XYBannerView(parentResponder, inputEventHandlerDelegate, textFieldDelegate),
  m_derivativeView(Font(), KDFont::ALIGN_CENTER, KDFont::ALIGN_CENTER, TextColor(), BackgroundColor()),
  m_tangentEquationView(Font(), I18n::Message::LinearRegressionFormula, KDFont::ALIGN_CENTER, KDFont::ALIGN_CENTER, TextColor(), BackgroundColor()),
  m_aView(Font(), KDFont::ALIGN_CENTER, KDFont::ALIGN_CENTER, TextColor(), BackgroundColor()),
  m_bView(Font(), KDFont::ALIGN_CENTER, KDFont::ALIGN_CENTER, TextColor(), BackgroundColor()),
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

bool BannerView::lineBreakBeforeSubview(View * subview) const {
  return subview == &m_tangentEquationView
      || Shared::XYBannerView::lineBreakBeforeSubview(subview);
}

}
