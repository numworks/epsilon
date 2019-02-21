#include "box_banner_view.h"
#include <assert.h>

namespace Statistics {

BoxBannerView::BoxBannerView() :
  m_seriesName(KDFont::SmallFont, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_calculationName(KDFont::SmallFont, I18n::Message::Minimum, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_calculationValue(KDFont::SmallFont, 1.0f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

View * BoxBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * subviews[] = {&m_seriesName, &m_calculationName, &m_calculationValue};
  return subviews[index];
}

}
