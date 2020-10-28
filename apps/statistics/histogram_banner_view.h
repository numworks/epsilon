#ifndef STATISTICS_HISTOGRAM_BANNER_VIEW_H
#define STATISTICS_HISTOGRAM_BANNER_VIEW_H

#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>
#include "../shared/banner_view.h"
#include <apps/i18n.h>

namespace Statistics {

class HistogramBannerView : public Shared::BannerView {
public:
  HistogramBannerView();
  Escher::BufferTextView * intervalView() { return &m_intervalView; }
  Escher::BufferTextView * sizeView() { return &m_sizeView; }
  Escher::BufferTextView * frequencyView() { return &m_frequencyView; }
private:
  static constexpr int k_numberOfSubviews = 6;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::View * subviewAtIndex(int index) override;
  Escher::MessageTextView m_intervalLegendView;
  Escher::BufferTextView m_intervalView;
  Escher::MessageTextView m_sizeLegendView;
  Escher::BufferTextView m_sizeView;
  Escher::MessageTextView m_frequencyLegendView;
  Escher::BufferTextView m_frequencyView;
};

}

#endif
