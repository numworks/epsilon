#include "banner_view.h"
#include <assert.h>

namespace Regression {

BannerView::BannerView(
  Responder * parentResponder,
  InputEventHandlerDelegate * inputEventHandlerDelegate,
  TextFieldDelegate * textFieldDelegate
) :
  Shared::XYBannerView(parentResponder, inputEventHandlerDelegate, textFieldDelegate),
  m_dotNameView(Font(), 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_regressionTypeView(Font(), (I18n::Message)0, 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_subText0(Font(), 0.5f, 0.5f, TextColor(), BackgroundColor()),
  m_subText1(Font(), 0.5f, 0.5f, TextColor(), BackgroundColor()),
  m_subText2(Font(), 0.5f, 0.5f, TextColor(), BackgroundColor()),
  m_subText3(Font(), 0.5f, 0.5f, TextColor(), BackgroundColor()),
  m_subText4(Font(), 0.5f, 0.5f, TextColor(), BackgroundColor())
{
}

BufferTextView * BannerView::subTextAtIndex(int index) {
  assert(0 <= index && index < numberOfsubTexts());
  BufferTextView * subTexts[numberOfsubTexts()] = {&m_subText0, &m_subText1, &m_subText2, &m_subText3, &m_subText4};
  return subTexts[index];
}

View * BannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  if (index == 0) {
    return &m_dotNameView;
  }
  index--;
  if (index < Shared::XYBannerView::k_numberOfSubviews) {
    return Shared::XYBannerView::subviewAtIndex(index);
  }
  index -= Shared::XYBannerView::k_numberOfSubviews;
  if (index == 0) {
    return &m_regressionTypeView;
  }
  return subTextAtIndex(index - 1);
}

}
