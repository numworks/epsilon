#ifndef STATISTICS_HISTOGRAM_BANNER_VIEW_H
#define STATISTICS_HISTOGRAM_BANNER_VIEW_H

#include <apps/i18n.h>
#include <apps/shared/banner_view.h>
#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>

namespace Statistics {

class HistogramBannerView : public Shared::BannerView {
 public:
  HistogramBannerView();
  BannerBufferTextView* seriesName() { return &m_seriesName; }
  BannerBufferTextView* intervalView() { return &m_intervalBuffer; }
  BannerBufferTextView* frequencyView() { return &m_frequencyBuffer; }
  BannerBufferTextView* relativeFrequencyView() {
    return &m_relativeFrequencyBuffer;
  }

 private:
  constexpr static int k_numberOfSubviews = 4;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::View* subviewAtIndex(int index) override;

  BannerBufferTextView m_seriesName;
  BannerBufferTextView m_intervalBuffer;
  BannerBufferTextView m_frequencyBuffer;
  BannerBufferTextView m_relativeFrequencyBuffer;
};

}  // namespace Statistics

#endif
