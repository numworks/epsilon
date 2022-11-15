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
  void setInterestMessage(I18n::Message message) { m_interestView.setMessage(message); }
  void setDisplayParameters(bool showInterest, bool showDerivative, bool showTangent);

private:
  int numberOfSubviews() const override {
    // there are 3 views for tangent (aView, bView, tangentEquationView)
    return XYBannerView::k_numberOfSubviews + hasInterestMessage() + m_showDerivative + 3 * m_showTangent;
  };
  Escher::View * subviewAtIndex(int index) override;
  bool lineBreakBeforeSubview(Escher::View * subview) const override;
  bool hasInterestMessage() const { return m_showInterest && m_interestView.text()[0] != '\0'; }

  Escher::MessageTextView m_interestView;
  Escher::BufferTextView m_derivativeView;
  Escher::MessageTextView m_tangentEquationView;
  Escher::BufferTextView m_aView;
  Escher::BufferTextView m_bView;
  bool m_showInterest : 1;
  bool m_showDerivative : 1;
  bool m_showTangent : 1;
};

}

#endif
