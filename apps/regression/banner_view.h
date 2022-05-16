#ifndef REGRESSION_BANNER_VIEW_H
#define REGRESSION_BANNER_VIEW_H

#include <escher/message_text_view.h>
#include "../shared/xy_banner_view.h"

namespace Regression {

/* This banner view displays cursor's x and y position as well as (optionally)
 * two other views :
 *  - m_otherView : A buffer text either postionned first or after "y="
 *  - m_displayDataNotSuitable : A message text displayed last */

// TODO Hugo : XYBannerView has TextField logic that are unused here. Optimize it.
class BannerView : public Shared::XYBannerView {
public:
  BannerView(
    Escher::Responder * parentResponder,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate
  );
  Escher::BufferTextView * otherView() { return &m_otherView; }
  void setDisplayParameters(bool displayOtherView, bool otherViewIsFirst, bool displayDataNotSuitable);

private:
  static constexpr int k_maxNumberOfSubviews = Shared::XYBannerView::k_numberOfSubviews + 2;

  int numberOfSubviews() const override { return k_maxNumberOfSubviews - !m_displayOtherView - !m_displayDataNotSuitable; }
  Escher::View * subviewAtIndex(int index) override;

  Escher::BufferTextView m_otherView;
  Escher::MessageTextView m_dataNotSuitableView;
  bool m_displayOtherView;
  bool m_otherViewIsFirst;
  bool m_displayDataNotSuitable;
};

}

#endif
