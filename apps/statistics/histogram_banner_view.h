#ifndef STATISTICS_HISTOGRAM_BANNER_VIEW_H
#define STATISTICS_HISTOGRAM_BANNER_VIEW_H

#include <escher.h>
#include "../shared/banner_view.h"

namespace Statistics {

class HistogramBannerView : public Shared::BannerView {
public:
  HistogramBannerView();
private:
  int numberOfSubviews() const override;
  TextView * textViewAtIndex(int i) const override;
  BufferTextView m_intervalView;
  BufferTextView m_sizeView;
  BufferTextView m_frequencyView;
};

}

#endif
