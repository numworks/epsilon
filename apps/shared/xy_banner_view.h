#ifndef SHARED_XY_BANNER_VIEW_H
#define SHARED_XY_BANNER_VIEW_H

#include "banner_view.h"
#include "editable_field_for_banner_view.h"

namespace Shared {

class XYBannerView : public BannerView, EditableFieldForBannerView {
 public:
  XYBannerView(Escher::Responder* parentResponder,
               Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
               Escher::TextFieldDelegate* textFieldDelegate)
      : EditableFieldForBannerView(parentResponder, inputEventHandlerDelegate,
                                   textFieldDelegate),
        m_ordinateView(k_font, KDContext::k_alignCenter,
                       KDContext::k_alignCenter, TextColor(),
                       BackgroundColor()) {}

  Escher::BufferTextView* abscissaSymbol() { return editableFieldLabel(); }
  Escher::TextField* abscissaValue() { return editableField(); }
  Escher::BufferTextView* ordinateView() { return &m_ordinateView; }
  constexpr static int k_numberOfSubviews = 2;

 protected:
  Escher::View* subviewAtIndex(int index) override {
    assert(0 <= index && index < k_numberOfSubviews);
    return index == 0 ? editablView() : &m_ordinateView;
  }

 private:
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::BufferTextView m_ordinateView;
};

}  // namespace Shared

#endif
