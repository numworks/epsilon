#include "banner_view.h"
#include <assert.h>

namespace Regression {

constexpr KDColor BannerView::k_textColor;
constexpr KDColor BannerView::k_backgroundColor;

BannerView::BannerView() :
  Shared::XYBannerView(),
  m_dotNameView(k_font, 0.0f, 0.5f, k_textColor, k_backgroundColor),
  m_regressionTypeView(k_font, (I18n::Message)0, 0.0f, 0.5f, k_textColor,k_backgroundColor),
  m_subText0(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_subText1(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_subText2(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_subText3(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor),
  m_subText4(k_font, 0.5f, 0.5f, k_textColor, k_backgroundColor)
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
