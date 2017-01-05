#include "histogram_banner_view.h"
#include "../constant.h"
#include <assert.h>
#include <poincare.h>

namespace Statistics {

HistogramBannerView::HistogramBannerView(Store * store) :
  m_intervalView(0.0f, 0.5f),
  m_sizeView(0.0f, 0.5f),
  m_frequencyView(1.0f, 0.5f),
  m_store(store)
{
}

void HistogramBannerView::reload() {
  markRectAsDirty(bounds());
  char buffer[k_maxNumberOfCharacters];
  const char * legend = "Interval [";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float lowerBound = m_store->selectedBar() - m_store->barWidth()/2;
  int lowerBoundNumberOfChar = Float(lowerBound).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  buffer[legendLength+lowerBoundNumberOfChar] = ';';
  float upperBound = m_store->selectedBar() + m_store->barWidth()/2;
  int upperBoundNumberOfChar = Float(upperBound).convertFloatToText(buffer+legendLength+lowerBoundNumberOfChar+1, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  buffer[legendLength+lowerBoundNumberOfChar+upperBoundNumberOfChar+1] = '[';
  buffer[legendLength+lowerBoundNumberOfChar+upperBoundNumberOfChar+2] = 0;
  m_intervalView.setText(buffer);

  legend = "Effectif: ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float size = m_store->heightForBarAtValue(m_store->selectedBar());
  Float(size).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_sizeView.setText(buffer);

  legend = "Frequence: ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float frequency = size/m_store->sumOfColumn(1);
  Float(frequency).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_frequencyView.setText(buffer);
}

int HistogramBannerView::numberOfSubviews() const {
  return 3;
}

View * HistogramBannerView::subviewAtIndex(int index) {
  assert(index >= 0);
  if (index == 0) {
    return &m_intervalView;
  }
  if (index == 1) {
    return &m_sizeView;
  }
  return &m_frequencyView;
}

void HistogramBannerView::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  m_intervalView.setFrame(KDRect(0, 0, width, height/2));
  m_sizeView.setFrame(KDRect(0, height/2, width, height/2));
  m_frequencyView.setFrame(KDRect(0, height/2, width, height/2));
}

}
