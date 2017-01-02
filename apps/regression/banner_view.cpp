#include "banner_view.h"
#include "../constant.h"
#include <assert.h>
#include <poincare.h>

namespace Regression {

BannerView::BannerView(Data * data) :
  m_regressionTypeView(nullptr, 0.5f, 0.5f),
  m_slopeView(0.5f, 0.5f),
  m_yInterceptView(0.5f, 0.5f),
  m_xView(0.5f, 0.5f),
  m_yView(0.5f, 0.5f),
  m_data(data)
{
}

void BannerView::reload() {
  markRectAsDirty(bounds());
  m_regressionTypeView.setText("y = ax+b");
  char buffer[k_maxNumberOfCharacters];
  const char * legend = "a = ";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float slope = m_data->slope();
  Float(slope).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_slopeView.setText(buffer);

  legend = "b = ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float yIntercept = m_data->yIntercept();
  Float(yIntercept).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_yInterceptView.setText(buffer);

  legend = "x = ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float x = m_data->xCursorPosition();
  Float(x).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_xView.setText(buffer);

  legend = "y = ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float y = m_data->yCursorPosition();
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
