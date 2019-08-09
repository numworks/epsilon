#include "xy_banner_view.h"
#include <assert.h>

namespace Shared {

XYBannerView::XYBannerView(
  Responder * parentResponder,
  InputEventHandlerDelegate * inputEventHandlerDelegate,
  TextFieldDelegate * textFieldDelegate
) :
  m_abscissaSymbol(Font(), 1.0f, 0.5f, TextColor(), BackgroundColor()),
  m_abscissaValue(
    parentResponder,
    m_textBody,
    k_abscissaBufferSize,
    k_abscissaBufferSize,
    inputEventHandlerDelegate,
    textFieldDelegate,
    Font(),
    0.0f, 0.5f,
    TextColor(),
    BackgroundColor()
  ),
  m_ordinateView(Font(), 0.5f, 0.5f, TextColor(), BackgroundColor())
{
}

View * XYBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * subviews[] = {&m_abscissaSymbol, &m_abscissaValue, &m_ordinateView};
  return subviews[index];
}

}
