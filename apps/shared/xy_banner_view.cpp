#include "xy_banner_view.h"

#include <assert.h>

using namespace Escher;

namespace Shared {

XYBannerView::XYBannerView(Responder* parentResponder,
                           InputEventHandlerDelegate* inputEventHandlerDelegate,
                           TextFieldDelegate* textFieldDelegate)
    : m_abscissaSymbol(k_font, KDContext::k_alignRight,
                       KDContext::k_alignCenter, TextColor(),
                       BackgroundColor()),
      m_abscissaValue(parentResponder, m_textBody, k_abscissaBufferSize,
                      TextField::MaxBufferSize(), inputEventHandlerDelegate,
                      textFieldDelegate, k_font, KDContext::k_alignLeft,
                      KDContext::k_alignCenter, TextColor(), BackgroundColor()),
      m_abscissaView(&m_abscissaSymbol, &m_abscissaValue),
      m_ordinateView(k_font, KDContext::k_alignCenter, KDContext::k_alignCenter,
                     TextColor(), BackgroundColor()) {
  m_textBody[0] = 0;
}

View* XYBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View* subviews[] = {&m_abscissaView, &m_ordinateView};
  return subviews[index];
}

}  // namespace Shared
