#ifndef REGRESSION_BANNER_VIEW_H
#define REGRESSION_BANNER_VIEW_H

#include <escher/message_text_view.h>
#include "../shared/xy_banner_view.h"

namespace Regression {

class BannerView : public Shared::XYBannerView {
public:
  BannerView(
    Escher::Responder * parentResponder,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate
  );
  Escher::BufferTextView * dotNameView() { return &m_dotNameView; }
  Escher::MessageTextView * regressionTypeView() { return &m_regressionTypeView; }
  Escher::BufferTextView * subTextAtIndex(int index);
  static constexpr int numberOfsubTexts() { return 5; }
private:
  static constexpr int k_numberOfSubviews = Shared::XYBannerView::k_numberOfSubviews + 7;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::View * subviewAtIndex(int index) override;
  Escher::BufferTextView m_dotNameView;
  Escher::MessageTextView m_regressionTypeView;
  Escher::BufferTextView m_subText0;
  Escher::BufferTextView m_subText1;
  Escher::BufferTextView m_subText2;
  Escher::BufferTextView m_subText3;
  Escher::BufferTextView m_subText4;
};

}

#endif
