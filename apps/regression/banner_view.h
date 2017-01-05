#ifndef REGRESSION_BANNER_VIEW_H
#define REGRESSION_BANNER_VIEW_H

#include <escher.h>
#include "store.h"
#include "../banner_view.h"

namespace Regression {

class BannerView : public ::BannerView {
public:
  BannerView(Store * store);
  void reload() override;
private:
  constexpr static int k_maxNumberOfCharacters = 50;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  PointerTextView m_regressionTypeView;
  BufferTextView m_slopeView;
  BufferTextView m_yInterceptView;
  BufferTextView m_xView;
  BufferTextView m_yView;
  Store * m_store;
};

}

#endif
