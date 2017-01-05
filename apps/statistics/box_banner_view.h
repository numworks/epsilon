#ifndef STATISTICS_BOX_BANNER_VIEW_H
#define STATISTICS_BOX_BANNER_VIEW_H

#include <escher.h>
#include "../banner_view.h"

namespace Statistics {

class BoxBannerView : public ::BannerView {
public:
  BoxBannerView();
private:
  int numberOfSubviews() const override;
  TextView * textViewAtIndex(int i) override;
  PointerTextView m_calculationName;
  BufferTextView m_calculationValue;
};

}

#endif
