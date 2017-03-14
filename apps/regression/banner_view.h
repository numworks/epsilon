#ifndef REGRESSION_BANNER_VIEW_H
#define REGRESSION_BANNER_VIEW_H

#include <escher.h>
#include "../shared/banner_view.h"

namespace Regression {

class BannerView : public Shared::BannerView {
public:
  BannerView();
private:
  int numberOfSubviews() const override;
  TextView * textViewAtIndex(int i) const override;
  PointerTextView * pointerTextViewAtIndex(int i) const override;
  PointerTextView m_regressionTypeView;
  BufferTextView m_slopeView;
  BufferTextView m_yInterceptView;
  BufferTextView m_xView;
  BufferTextView m_yView;
};

}

#endif
