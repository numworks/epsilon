#ifndef STATISTICS_BOX_BANNER_VIEW_H
#define STATISTICS_BOX_BANNER_VIEW_H

#include <escher.h>
#include "store.h"
#include "../banner_view.h"

namespace Statistics {

class BoxView;

class BoxBannerView : public ::BannerView {
public:
  BoxBannerView(Store * store, BoxView * boxView);
  void reload() override;
private:
  constexpr static int k_maxNumberOfCharacters = 50;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  PointerTextView m_calculationName;
  BufferTextView m_calculationValue;
  Store * m_store;
  BoxView * m_boxView;
};

}

#endif
