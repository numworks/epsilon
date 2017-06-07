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
  MessageTextView * messageTextViewAtIndex(int i) const override;
  BufferTextView m_dotNameView;
  BufferTextView m_xView;
  BufferTextView m_yView;
  MessageTextView m_regressionTypeView;
  BufferTextView m_slopeView;
  BufferTextView m_yInterceptView;
  BufferTextView m_rView;
  BufferTextView m_r2View;
};

}

#endif
