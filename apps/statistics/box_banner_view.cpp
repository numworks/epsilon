#include "box_banner_view.h"

namespace Statistics {

BoxBannerView::BoxBannerView() :
  m_calculationName(nullptr, 0.0f, 0.5f),
  m_calculationValue(1.0f, 0.5f)
{
}

int BoxBannerView::numberOfSubviews() const {
  return 2;
}

TextView * BoxBannerView::textViewAtIndex(int index) {
  TextView * textViews[2] = {&m_calculationName, &m_calculationValue};
  return textViews[index];
}

}
