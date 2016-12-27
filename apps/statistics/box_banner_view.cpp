#include "box_banner_view.h"
#include "box_view.h"
#include "../constant.h"
#include <assert.h>
#include <poincare.h>

namespace Statistics {

BoxBannerView::BoxBannerView(Data * data, BoxView * boxView) :
  m_calculationName(nullptr, 0.0f, 0.5f),
  m_calculationValue(1.0f, 0.5f),
  m_data(data),
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
      calculation = m_data->minValue();
      break;
    case 1:
      calculation = m_data->firstQuartile();
      break;
    case 2:
      calculation = m_data->median();
      break;
    case 3:
      calculation = m_data->thirdQuartile();
      break;
    case 4:
      calculation = m_data->maxValue();
      break;
  }
  Float(calculation).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_calculationValue.setText(buffer);
}

void BoxBannerView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
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
