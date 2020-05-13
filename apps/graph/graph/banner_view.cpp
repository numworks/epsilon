#include "banner_view.h"
#include <assert.h>
#include <apps/i18n.h>

namespace Graph {

BannerView::BannerView(
  Responder * parentResponder,
  InputEventHandlerDelegate * inputEventHandlerDelegate,
  TextFieldDelegate * textFieldDelegate
) :
  Shared::XYBannerView(parentResponder, inputEventHandlerDelegate, textFieldDelegate),
  m_derivativeView(Font(), 0.5f, 0.5f, TextColor(), BackgroundColor()),
  m_tangentEquationView(Font(), I18n::Message::LinearRegressionFormula, 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_aView(Font(), 0.5f, 0.5f, TextColor(), BackgroundColor()),
  m_bView(Font(), 0.5f, 0.5f, TextColor(), BackgroundColor()),
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
