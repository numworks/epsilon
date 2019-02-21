#ifndef REGRESSION_BANNER_VIEW_H
#define REGRESSION_BANNER_VIEW_H

#include <escher.h>
#include "../shared/banner_view.h"

namespace Regression {

class BannerView : public Shared::BannerView {
public:
  BannerView();
  int numberOfTextviews() const { return k_numberOfSubviews; }
  const KDFont * font() const { return k_font; }
private:
  static constexpr const KDFont * k_font = KDFont::SmallFont;
  static constexpr KDColor k_textColor = KDColorBlack;
  static constexpr KDColor k_backgroundColor = Palette::GreyMiddle;
  static constexpr int k_numberOfSubviews = 9;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  View * subviewAtIndex(int index) override;
  MessageTextView * messageTextViewAtIndex(int i) const override;
  BufferTextView m_dotNameView;
  BufferTextView m_xView;
  BufferTextView m_yView;
  MessageTextView m_regressionTypeView;
  BufferTextView m_subText1;
  BufferTextView m_subText2;
  BufferTextView m_subText3;
  BufferTextView m_subText4;
  BufferTextView m_subText5;
};

}

#endif
