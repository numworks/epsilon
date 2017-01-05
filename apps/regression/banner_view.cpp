#include "banner_view.h"
#include "../constant.h"
#include <assert.h>
#include <poincare.h>

namespace Regression {

BannerView::BannerView(Store * store) :
  m_regressionTypeView(nullptr, 0.5f, 0.5f),
  m_slopeView(0.5f, 0.5f),
  m_yInterceptView(0.5f, 0.5f),
  m_xView(0.5f, 0.5f),
  m_yView(0.5f, 0.5f),
  m_store(store)
{
}

void BannerView::reload() {
  markRectAsDirty(bounds());
  m_regressionTypeView.setText("y = ax+b");
  char buffer[k_maxNumberOfCharacters];
  const char * legend = "a = ";
  float slope = m_store->slope();
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  Float(slope).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_slopeView.setText(buffer);

  legend = "b = ";
  float yIntercept = m_store->yIntercept();
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  Float(yIntercept).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_yInterceptView.setText(buffer);

  legend = "x = ";
  float x = m_store->xCursorPosition();
  // Display a specific legend if the mean dot is selected
  if (m_store->selectedDotIndex() == m_store->numberOfPairs()) {
    legend = "x^ = ";
    x = m_store->meanOfColumn(0);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  Float(x).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_xView.setText(buffer);

  legend = "y = ";
  float y = m_store->yCursorPosition();
  if (m_store->selectedDotIndex() == m_store->numberOfPairs()) {
    legend = "y^ = ";
    y = m_store->meanOfColumn(1);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  Float(y).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_yView.setText(buffer);
}

int BannerView::numberOfSubviews() const {
  return 5;
}

View * BannerView::subviewAtIndex(int index) {
  assert(index >= 0);
  switch (index) {
    case 0:
      return &m_regressionTypeView;
    case 1:
      return &m_slopeView;
    case 2:
      return &m_yInterceptView;
    case 3:
      return &m_xView;
    case 4:
      return &m_yView;
    default:
      assert(false);
      return nullptr;
  }
}

void BannerView::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  m_regressionTypeView.setFrame(KDRect(0, 0, width/3, height/2));
  m_slopeView.setFrame(KDRect(width/3, 0, width/3, height/2));
  m_yInterceptView.setFrame(KDRect(2*width/3, 0, width/3, height/2));
  m_xView.setFrame(KDRect(0, height/2, width/3, height/2));
  m_yView.setFrame(KDRect(width/3, height/2, width/3, height/2));
}

}
