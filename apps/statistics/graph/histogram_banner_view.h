#ifndef STATISTICS_HISTOGRAM_BANNER_VIEW_H
#define STATISTICS_HISTOGRAM_BANNER_VIEW_H

#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>
#include <apps/shared/banner_view.h>
#include <apps/i18n.h>

namespace Statistics {

class HistogramBannerView : public Shared::BannerView {
public:
  HistogramBannerView();
  Escher::BufferTextView * seriesName() { return &m_seriesName; }
  Escher::BufferTextView * intervalView() { return &m_intervalBuffer; }
  Escher::BufferTextView * frequencyView() { return &m_frequencyBuffer; }
  Escher::BufferTextView * relativeFrequencyView() { return &m_relativeFrequencyBuffer; }
private:
  static constexpr int k_numberOfSubviews = 4;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::View * subviewAtIndex(int index) override;

  Escher::BufferTextView m_seriesName;
  Escher::BufferTextView m_intervalBuffer;
  Escher::BufferTextView m_frequencyBuffer;
  Escher::BufferTextView m_relativeFrequencyBuffer;
};

}

#endif
