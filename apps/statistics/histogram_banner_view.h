#ifndef STATISTICS_HISTOGRAM_BANNER_VIEW_H
#define STATISTICS_HISTOGRAM_BANNER_VIEW_H

#include <escher.h>
#include "../shared/banner_view.h"
#include <apps/i18n.h>

namespace Statistics {

class HistogramBannerView : public Shared::BannerView {
public:
  HistogramBannerView();
  BufferTextView * intervalView() { return &m_intervalView; }
  BufferTextView * sizeView() { return &m_sizeView; }
  BufferTextView * frequencyView() { return &m_frequencyView; }
private:
  static constexpr int k_numberOfSubviews = 6;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  View * subviewAtIndex(int index) override;
  MessageTextView m_intervalLegendView;
  BufferTextView m_intervalView;
  MessageTextView m_sizeLegendView;
  BufferTextView m_sizeView;
  MessageTextView m_frequencyLegendView;
  BufferTextView m_frequencyView;
};

}

#endif
