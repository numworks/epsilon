#ifndef REGRESSION_BANNER_VIEW_H
#define REGRESSION_BANNER_VIEW_H

#include <escher/message_text_view.h>
#include "../shared/xy_banner_view.h"

namespace Regression {

class BannerView : public Shared::XYBannerView {
public:
  static constexpr int k_numberOfSharedSubviews = 2;

  BannerView(
    Escher::Responder * parentResponder,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate
  );
  Escher::BufferTextView * dotNameView() { return &m_dotNameView; }
  Escher::MessageTextView * regressionTypeView() { return &m_regressionTypeView; }
  Escher::BufferTextView * subTextAtIndex(int index);
  static constexpr int numberOfsubTexts() { return k_numberOfSubtexts; }
  void setNumberOfSubviews(int subviewsNumber) { m_numberOfSubviews = subviewsNumber; }
  void setCoefficientsDefined(bool defined) { m_coefficientsAreDefined = defined; }
private:
  static constexpr int k_numberOfSubtexts = 5;
  static constexpr int k_maxNumberOfSubviews = Shared::XYBannerView::k_numberOfSubviews + k_numberOfSharedSubviews + k_numberOfSubtexts;
  int numberOfSubviews() const override { return m_numberOfSubviews; }
  Escher::View * subviewAtIndex(int index) override;
  bool lineBreakBeforeSubview(Escher::View * subview) const override;
  Escher::BufferTextView m_dotNameView;
  Escher::MessageTextView m_regressionTypeView;
  Escher::BufferTextView m_subText0;
  Escher::BufferTextView m_subText1;
  Escher::BufferTextView m_subText2;
  Escher::BufferTextView m_subText3;
  Escher::BufferTextView m_subText4;
  int m_numberOfSubviews;
  bool m_coefficientsAreDefined;
};

}

#endif
