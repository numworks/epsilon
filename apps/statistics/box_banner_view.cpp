#include "box_banner_view.h"

namespace Statistics {

BoxBannerView::BoxBannerView() :
  m_seriesName(KDText::FontSize::Small, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_calculationName(KDText::FontSize::Small, I18n::Message::Minimum, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_calculationValue(KDText::FontSize::Small, 1.0f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

TextView * BoxBannerView::textViewAtIndex(int index) const {
  const TextView * textViews[3] = {&m_seriesName, &m_calculationName, &m_calculationValue};
  return (TextView *)textViews[index];
}

MessageTextView * BoxBannerView::messageTextViewAtIndex(int index) const {
  return index == 1 ? (MessageTextView *)&m_calculationName : nullptr;
}

}
