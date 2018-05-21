#include "box_banner_view.h"

namespace Statistics {

BoxBannerView::BoxBannerView() :
  m_calculationName(KDText::FontSize::Small, I18n::Message::Minimum, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_calculationValue(KDText::FontSize::Small, 1.0f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

TextView * BoxBannerView::textViewAtIndex(int index) const {
  const TextView * textViews[2] = {&m_calculationName, &m_calculationValue};
  return (TextView *)textViews[index];
}

MessageTextView * BoxBannerView::messageTextViewAtIndex(int index) const {
  return index == 0 ? (MessageTextView *)&m_calculationName : nullptr;
}

}
