#ifndef STATISTICS_HISTOGRAM_BANNER_VIEW_H
#define STATISTICS_HISTOGRAM_BANNER_VIEW_H

#include <escher.h>
#include "../shared/banner_view.h"
#include "../i18n.h"

namespace Statistics {

class HistogramBannerView : public Shared::BannerView {
public:
  HistogramBannerView();
private:
  int numberOfSubviews() const override;
  TextView * textViewAtIndex(int i) const override;
  PointerTextView * pointerTextViewAtIndex(int index) const override;
  PointerTextView m_intervalLegendView;
  BufferTextView m_intervalView;
  PointerTextView m_sizeLegendView;
  BufferTextView m_sizeView;
  PointerTextView m_frequencyLegendView;
  BufferTextView m_frequencyView;
};

}

#endif
