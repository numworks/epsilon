#ifndef GRAPH_BANNER_VIEW_H
#define GRAPH_BANNER_VIEW_H

#include <escher/message_text_view.h>
#include "../../shared/xy_banner_view.h"

namespace Graph {

class BannerView : public Shared::XYBannerView {
public:
  BannerView(
    Escher::Responder * parentResponder,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate
  );
  Escher::BufferTextView * derivativeView() { return &m_derivativeView; }
  Escher::BufferTextView * aView() { return &m_aView; }
  Escher::BufferTextView * bView() { return &m_bView; }
  void setNumberOfSubviews(int numberOfSubviews) { m_numberOfSubviews = numberOfSubviews; }
  constexpr static int k_numberOfSubviews = Shared::XYBannerView::k_numberOfSubviews + 4;
private:
  int numberOfSubviews() const override { return m_numberOfSubviews; }
  bool lineBreakBeforeSubview(Escher::View * subview) const override;
  Escher::View * subviewAtIndex(int index) override;
  Escher::BufferTextView m_derivativeView;
  Escher::MessageTextView m_tangentEquationView;
  Escher::BufferTextView m_aView;
  Escher::BufferTextView m_bView;
  int m_numberOfSubviews;
};

}

#endif
