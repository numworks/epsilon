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
  m_derivativeView(k_font, KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
  m_tangentEquationView(k_font, I18n::Message::LinearRegressionFormula, KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
  m_aView(k_font, KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
  m_bView(k_font, KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
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
