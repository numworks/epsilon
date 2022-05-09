#ifndef REGRESSION_BANNER_VIEW_H
#define REGRESSION_BANNER_VIEW_H

#include <escher/message_text_view.h>
#include "../shared/xy_banner_view.h"

namespace Regression {

class BannerView : public Shared::XYBannerView {
public:
  BannerView(
    Escher::Responder * parentResponder,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate
  );
  Escher::BufferTextView * otherView() { return &m_otherView; }
  // The other view may either be hidden, displayed first, or displayed last
  void setDisplayParameters(bool hideOtherView, bool otherViewIsFirst);
private:
  static constexpr int k_maxNumberOfSubviews = Shared::XYBannerView::k_numberOfSubviews + 1;
  int numberOfSubviews() const override { return m_numberOfSubviews; }
  Escher::View * subviewAtIndex(int index) override;
  Escher::BufferTextView m_otherView;
  bool m_otherViewIsFirst;
  int m_numberOfSubviews;
};

}

#endif
