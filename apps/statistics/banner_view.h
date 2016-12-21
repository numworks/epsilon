#ifndef STATISTICS_BANNER_VIEW_H
#define STATISTICS_BANNER_VIEW_H

#include <escher.h>
#include "data.h"

namespace Statistics {

class BannerView : public View {
public:
  BannerView(Data * data);
  void reload();
  void drawRect(KDContext * ctx, KDRect rect) const override;
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
