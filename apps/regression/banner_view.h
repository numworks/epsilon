#ifndef REGRESSION_BANNER_VIEW_H
#define REGRESSION_BANNER_VIEW_H

#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>
#include "xy_banner_view.h"

namespace Regression {

/* This banner view displays cursor's x and y position as well as (optionally)
 * two other views :
 *  - m_otherView : A buffer text either postionned first or after ordinateView
 *  - m_displayDataNotSuitable : A message text displayed last */

class BannerView : public XYBannerView {
public:
  BannerView();
  Escher::BufferTextView * otherView() { return &m_otherView; }
  void setDisplayParameters(bool displayOtherView, bool otherViewIsFirst, bool displayDataNotSuitable);

private:
  int numberOfSubviews() const override { return XYBannerView::k_numberOfSubviews + m_displayOtherView + m_displayDataNotSuitable; }
  Escher::View * subviewAtIndex(int index) override;

  Escher::BufferTextView m_otherView;
  Escher::MessageTextView m_dataNotSuitableView;
  bool m_displayOtherView;
  bool m_otherViewIsFirst;
  bool m_displayDataNotSuitable;
};

}

#endif
