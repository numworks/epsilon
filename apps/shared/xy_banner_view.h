#ifndef SHARED_XY_BANNER_VIEW_H
#define SHARED_XY_BANNER_VIEW_H

#include "banner_view.h"
#include <poincare/print_float.h>

namespace Shared {

class XYBannerView : public BannerView {
public:
  XYBannerView(
    Responder * parentResponder,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    TextFieldDelegate * textFieldDelegate
  );
  BufferTextView * abscissaSymbol() { return &m_abscissaSymbol; }
  TextField * abscissaValue() { return &m_abscissaValue; }
  BufferTextView * ordinateView() { return &m_ordinateView; }
  static constexpr int k_numberOfSubviews = 3;
protected:
  View * subviewAtIndex(int index) override;
private:
  constexpr static KDCoordinate k_abscissaBufferSize = Poincare::PrintFloat::k_maxFloatCharSize;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  BufferTextView m_abscissaSymbol;
  TextField m_abscissaValue;
  char m_textBody[k_abscissaBufferSize];
  BufferTextView m_ordinateView;
};

}

#endif
