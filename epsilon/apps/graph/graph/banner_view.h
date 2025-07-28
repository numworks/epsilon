#ifndef GRAPH_BANNER_VIEW_H
#define GRAPH_BANNER_VIEW_H

#include <apps/shared/cursor_view.h>
#include <apps/shared/xy_banner_view.h>
#include <escher/message_text_view.h>

namespace Graph {

class BannerView : public Shared::XYBannerView {
 public:
  BannerView(Escher::Responder* parentResponder,
             Escher::TextFieldDelegate* textFieldDelegate);

  struct DisplayParameters {
    bool showInterest : 1;
    bool showOrdinate : 1;
    bool showFirstDerivative : 1;
    bool showSecondDerivative : 1;
    bool showSlope : 1;
    bool showTangent : 1;
  };

  BannerBufferTextView* firstDerivativeView() { return &m_firstDerivativeView; }
  BannerBufferTextView* secondDerivativeView() {
    return &m_secondDerivativeView;
  }
  BannerBufferTextView* slopeView() { return &m_slopeView; }
  BannerBufferTextView* aView() { return &m_aView; }
  BannerBufferTextView* bView() { return &m_bView; }
  int numberOfInterestMessages() const;
  void addInterestMessage(I18n::Message message);
  void emptyInterestMessages();
  void setDisplayParameters(DisplayParameters displayParameters);
  bool showFirstDerivative() const {
    return m_displayParameters.showFirstDerivative;
  }
  bool showSlope() const { return m_displayParameters.showSlope; }

 private:
  constexpr static int k_maxNumberOfInterests = 3;

  int numberOfSubviews() const override {
    return numberOfInterestMessages()                  // interests messages
           + 1                                         // x
           + m_displayParameters.showOrdinate          // f(x)
           + m_displayParameters.showFirstDerivative   // f'(x)
           + m_displayParameters.showSecondDerivative  // f"(x)
           + m_displayParameters.showSlope             // dy/dx
           + 3 * m_displayParameters.showTangent;      // (a, b, y=ax+b)
  };
  Escher::View* subviewAtIndex(int index) override;
  bool lineBreakBeforeSubview(Escher::View* subview) const override;
  bool hasInterestMessage(int i) const {
    assert(i >= 0 && i < k_maxNumberOfInterests);
    return m_displayParameters.showInterest &&
           m_interestMessageView[i].text()[0] != '\0';
  }

  Escher::MessageTextView m_interestMessageView[k_maxNumberOfInterests];
  BannerBufferTextView m_firstDerivativeView;
  BannerBufferTextView m_secondDerivativeView;
  BannerBufferTextView m_slopeView;
  Escher::MessageTextView m_tangentEquationView;
  BannerBufferTextView m_aView;
  BannerBufferTextView m_bView;
  DisplayParameters m_displayParameters;
};

}  // namespace Graph

#endif
