#include "xy_banner_view.h"
#include <assert.h>

namespace Shared {

XYBannerView::XYBannerView(
  Responder * parentResponder,
  InputEventHandlerDelegate * inputEventHandlerDelegate,
  TextFieldDelegate * textFieldDelegate
) :
  m_abscissaView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_ordinateView(KDFont::SmallFont, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

View * XYBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * subviews[] = {&m_abscissaView, &m_ordinateView};
  return subviews[index];
}

}
