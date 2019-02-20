#ifndef STATISTICS_BOX_BANNER_VIEW_H
#define STATISTICS_BOX_BANNER_VIEW_H

#include <escher.h>
#include "../shared/banner_view.h"
#include <apps/i18n.h>

namespace Statistics {

class BoxBannerView : public Shared::BannerView {
public:
  BoxBannerView();
private:
  static constexpr int k_numberOfSubviews = 3;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  TextView * textViewAtIndex(int i) const override;
  MessageTextView * messageTextViewAtIndex(int i) const override;
  BufferTextView m_seriesName;
  MessageTextView m_calculationName;
  BufferTextView m_calculationValue;
};

}

#endif
