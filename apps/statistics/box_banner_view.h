#ifndef STATISTICS_BOX_BANNER_VIEW_H
#define STATISTICS_BOX_BANNER_VIEW_H

#include <escher.h>
#include "data.h"

namespace Statistics {

class BoxView;

class BoxBannerView : public View {
public:
  BoxBannerView(Data * data, BoxView * boxView);
  void reload();
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static int k_maxNumberOfCharacters = 50;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  PointerTextView m_calculationName;
  BufferTextView m_calculationValue;
  Data * m_data;
  BoxView * m_boxView;
};

}

#endif
