#ifndef STATISTICS_HISTOGRAM_BANNER_VIEW_H
#define STATISTICS_HISTOGRAM_BANNER_VIEW_H

#include <escher.h>
#include "data.h"
#include "../banner_view.h"

namespace Statistics {

class HistogramBannerView : public ::BannerView {
public:
  HistogramBannerView(Data * data);
  void reload() override;
private:
  constexpr static int k_maxNumberOfCharacters = 50;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  BufferTextView m_intervalView;
  BufferTextView m_sizeView;
  BufferTextView m_frequencyView;
  Data * m_data;
};

}

#endif
