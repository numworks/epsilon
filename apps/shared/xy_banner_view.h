#ifndef SHARED_XY_BANNER_VIEW_H
#define SHARED_XY_BANNER_VIEW_H

#include <escher/buffer_text_view.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <poincare/print_float.h>

#include "banner_view.h"

namespace Shared {

class XYBannerView : public BannerView {
 public:
  XYBannerView(Escher::Responder* parentResponder,
               Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
               Escher::TextFieldDelegate* textFieldDelegate);
  Escher::BufferTextView* abscissaSymbol() { return &m_abscissaSymbol; }
  Escher::TextField* abscissaValue() { return &m_abscissaValue; }
  Escher::BufferTextView* ordinateView() { return &m_ordinateView; }
  constexpr static int k_numberOfSubviews = 2;

 protected:
  Escher::View* subviewAtIndex(int index) override;

 private:
  constexpr static KDCoordinate k_abscissaBufferSize =
      Poincare::PrintFloat::k_maxFloatCharSize;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::BufferTextView m_abscissaSymbol;
  Escher::TextField m_abscissaValue;
  BannerView::LabelledView m_abscissaView;
  char m_textBody[k_abscissaBufferSize];
  Escher::BufferTextView m_ordinateView;
};

}  // namespace Shared

#endif
