#include "box_banner_view.h"
#include "box_view.h"
#include "../constant.h"
#include <assert.h>
#include <poincare.h>

namespace Statistics {

BoxBannerView::BoxBannerView(Store * store, BoxView * boxView) :
  m_calculationName(nullptr, 0.0f, 0.5f),
  m_calculationValue(1.0f, 0.5f),
  m_store(store),
  m_boxView(boxView)
{
}

void BoxBannerView::reload() {
  markRectAsDirty(bounds());
  const char * calculationName[5] = {"Minimum", "Premier quartile", "Mediane", "Troisieme quartile", "Maximum"};
  m_calculationName.setText(calculationName[m_boxView->selectedQuantile()]);

  char buffer[Constant::FloatBufferSizeInScientificMode];
  float calculation = 0.0f;
  switch(m_boxView->selectedQuantile()) {
    case 0:
      calculation = m_store->minValue();
      break;
    case 1:
      calculation = m_store->firstQuartile();
      break;
    case 2:
      calculation = m_store->median();
      break;
    case 3:
      calculation = m_store->thirdQuartile();
      break;
    case 4:
      calculation = m_store->maxValue();
      break;
  }
  Float(calculation).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_calculationValue.setText(buffer);
}

int BoxBannerView::numberOfSubviews() const {
  return 2;
}

View * BoxBannerView::subviewAtIndex(int index) {
  assert(index == 0 || index == 1);
  if (index == 0) {
    return &m_calculationName;
  }
  return &m_calculationValue;
}

void BoxBannerView::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  m_calculationName.setFrame(KDRect(0, 0, width/2, height));
  m_calculationValue.setFrame(KDRect(width/2, 0, width/2, height));
}

}
