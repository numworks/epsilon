#ifndef REGRESSION_BANNER_VIEW_H
#define REGRESSION_BANNER_VIEW_H

#include "../shared/xy_banner_view.h"

namespace Regression {

class BannerView : public Shared::XYBannerView {
public:
  BannerView(
    Responder * parentResponder,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    TextFieldDelegate * textFieldDelegate
  );
  BufferTextView * dotNameView() { return &m_dotNameView; }
  MessageTextView * regressionTypeView() { return &m_regressionTypeView; }
  BufferTextView * subTextAtIndex(int index);
  static constexpr int numberOfsubTexts() { return 5; }
private:
  static constexpr int k_numberOfSubviews = Shared::XYBannerView::k_numberOfSubviews + 7;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  View * subviewAtIndex(int index) override;
  BufferTextView m_dotNameView;
  MessageTextView m_regressionTypeView;
  BufferTextView m_subText0;
  BufferTextView m_subText1;
  BufferTextView m_subText2;
  BufferTextView m_subText3;
  BufferTextView m_subText4;
};

}

#endif
