#ifndef STATISTICS_BOX_BANNER_VIEW_H
#define STATISTICS_BOX_BANNER_VIEW_H

#include <escher.h>
#include "../shared/banner_view.h"
#include "../i18n.h"

namespace Statistics {

class BoxBannerView : public Shared::BannerView {
public:
  BoxBannerView();
private:
  int numberOfSubviews() const override;
  TextView * textViewAtIndex(int i) const override;
  MessageTextView * messageTextViewAtIndex(int i) const override;
  MessageTextView m_calculationName;
  BufferTextView m_calculationValue;
};

}

#endif
