#include "histogram_controller.h"
#include "../apps_container.h"
#include "../shared/poincare_helpers.h"
#include "app.h"
#include <cmath>
#include <assert.h>
#include <float.h>

using namespace Poincare;
using namespace Shared;

namespace Statistics {

static inline float minFloat(float x, float y) { return x < y ? x : y; }
static inline float maxFloat(float x, float y) { return x > y ? x : y; }

HistogramController::HistogramController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, Store * store, uint32_t * storeVersion, uint32_t * barVersion, uint32_t * rangeVersion, int * selectedBarIndex, int * selectedSeriesIndex) :
  MultipleDataViewController(parentResponder, store, selectedBarIndex, selectedSeriesIndex),
  ButtonRowDelegate(header, nullptr),
  m_view(this, store),
  m_storeVersion(storeVersion),
  m_barVersion(barVersion),
  m_rangeVersion(rangeVersion),
  m_histogramParameterController(nullptr, inputEventHandlerDelegate, store)
{
}

void HistogramController::setCurrentDrawnSeries(int series) {
  initYRangeParameters(series);
}

StackViewController * HistogramController::stackController() {
  StackViewController * stack = (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
  return stack;
}

const char * HistogramController::title() {
  return I18n::translate(I18n::Message::HistogramTab);
}

bool HistogramController::handleEvent(Ion::Events::Event event) {
  assert(selectedSeriesIndex() >= 0);
  if (event == Ion::Events::OK) {
    stackController()->push(histogramParameterController());
    return true;
  }
  return MultipleDataViewController::handleEvent(event);
}

void HistogramController::viewWillAppear() {
  MultipleDataViewController::viewWillAppear();
  uint32_t storeChecksum = m_store->storeChecksum();
  if (*m_storeVersion != storeChecksum) {
    *m_storeVersion = storeChecksum;
    initBarParameters();
    initRangeParameters();
  }
  uint32_t barChecksum = m_store->barChecksum();
  if (*m_barVersion != barChecksum) {
    *m_barVersion = barChecksum;
    initRangeParameters();
  }
  uint32_t rangeChecksum = m_store->rangeChecksum();
  if (*m_rangeVersion != rangeChecksum) {
    *m_rangeVersion = rangeChecksum;
    initBarSelection();
    reloadBannerView();
  }
}

void HistogramController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == nullptr || nextFirstResponder == tabController()) {
    if (selectedSeriesIndex() >= 0) {
      m_view.dataViewAtIndex(selectedSeriesIndex())->setForceOkDisplay(false);
    }
  }
  MultipleDataViewController::willExitResponderChain(nextFirstResponder);
}

void HistogramController::highlightSelection() {
  HistogramView * selectedHistogramView = static_cast<HistogramView *>(m_view.dataViewAtIndex(selectedSeriesIndex()));
  selectedHistogramView->setHighlight(m_store->startOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex), m_store->endOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex));
  // if the selectedBar was outside of range, we need to scroll
  if (m_store->scrollToSelectedBarIndex(selectedSeriesIndex(), *m_selectedBarIndex)) {
    multipleDataView()->reload();
  }
}

Responder * HistogramController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

void HistogramController::reloadBannerView() {
  if (selectedSeriesIndex() < 0) {
    return;
  }
  const size_t bufferSize = k_maxNumberOfCharacters+ PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)*2;
  char buffer[bufferSize];
  int numberOfChar = 0;

  // Add Interval Data
  const char * legend = " [";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, bufferSize);
  numberOfChar += legendLength;

  // Add lower bound
  if (selectedSeriesIndex() >= 0) {
    double lowerBound = m_store->startOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex);
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(lowerBound, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  }

  buffer[numberOfChar++] = ';';

  // Add upper bound
  if (selectedSeriesIndex() >= 0) {
    double upperBound = m_store->endOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex);
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(upperBound, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  }
  buffer[numberOfChar++] = '[';

  // Padding
  for (int i = numberOfChar; i < k_maxIntervalLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxIntervalLegendLength] = 0;
  m_view.editableBannerView()->setLegendAtIndex(buffer, 1);

  // Add Size Data
  numberOfChar = 0;
  legend = ": ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, bufferSize);
  numberOfChar += legendLength;
  double size = 0;
  if (selectedSeriesIndex() >= 0) {
    size = m_store->heightOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex);
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(size, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  }
  // Padding
  for (int i = numberOfChar; i < k_maxLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxLegendLength] = 0;
  m_view.editableBannerView()->setLegendAtIndex(buffer, 3);

  // Add Frequency Data
  numberOfChar = 0;
  legend = ": ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, bufferSize);
  numberOfChar += legendLength;
  if (selectedSeriesIndex() >= 0) {
    double frequency = size/m_store->sumOfOccurrences(selectedSeriesIndex());
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(frequency, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  }
  // Padding
  for (int i = numberOfChar; i < k_maxLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxLegendLength] = 0;
  m_view.editableBannerView()->setLegendAtIndex(buffer, 5);
}

bool HistogramController::moveSelectionHorizontally(int deltaIndex) {
  int newSelectedBarIndex = *m_selectedBarIndex;
  do {
    newSelectedBarIndex+=deltaIndex;
  } while (m_store->heightOfBarAtIndex(selectedSeriesIndex(), newSelectedBarIndex) == 0
      && newSelectedBarIndex >= 0
      && newSelectedBarIndex < m_store->numberOfBars(selectedSeriesIndex()));

  if (newSelectedBarIndex >= 0
      && newSelectedBarIndex < m_store->numberOfBars(selectedSeriesIndex())
      && *m_selectedBarIndex != newSelectedBarIndex)
  {
    *m_selectedBarIndex = newSelectedBarIndex;
    m_view.dataViewAtIndex(selectedSeriesIndex())->setHighlight(m_store->startOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex), m_store->endOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex));
    if (m_store->scrollToSelectedBarIndex(selectedSeriesIndex(), *m_selectedBarIndex)) {
      multipleDataView()->reload();
    }
    reloadBannerView();
    return true;
  }
  return false;
}

void HistogramController::initRangeParameters() {
  assert(selectedSeriesIndex() >= 0 && m_store->sumOfOccurrences(selectedSeriesIndex()) > 0);
  float minValue = m_store->firstDrawnBarAbscissa();
  float maxValue = -FLT_MAX;
  for (int i = 0; i < Store::k_numberOfSeries; i ++) {
    if (!m_store->seriesIsEmpty(i)) {
      maxValue = maxFloat(maxValue, m_store->maxValue(i));
    }
  }
  float barWidth = m_store->barWidth();
  float xMin = minValue;
  float xMax = maxValue + barWidth;
  /* if a bar is represented by less than one pixel, we cap xMax */
  if ((xMax - xMin)/barWidth > k_maxNumberOfBarsPerWindow) {
    xMax = xMin + k_maxNumberOfBarsPerWindow*barWidth;
  }
  /* Edge case */
  if (xMin >= xMax) {
    xMax = xMin + 10.0f*barWidth;
  }
  m_store->setXMin(xMin - Store::k_displayLeftMarginRatio*(xMax-xMin));
  m_store->setXMax(xMax + Store::k_displayRightMarginRatio*(xMax-xMin));

  initYRangeParameters(selectedSeriesIndex());
}

void HistogramController::initYRangeParameters(int series) {
  assert(series >= 0 && m_store->sumOfOccurrences(series) > 0);
  float yMax = -FLT_MAX;
  for (int index = 0; index < m_store->numberOfBars(series); index++) {
    float size = m_store->heightOfBarAtIndex(series, index);
    if (size > yMax) {
      yMax = size;
    }
  }
  yMax = yMax/m_store->sumOfOccurrences(series);
  yMax = yMax < 0 ? 1 : yMax;
  m_store->setYMax(yMax*(1.0f+Store::k_displayTopMarginRatio));

  /* Compute YMin:
   *    ratioFloatPixel*(0-yMin) = k_bottomMargin
   *    ratioFloatPixel*(yMax-yMin) = viewHeight
   *
   *    -ratioFloatPixel*yMin = k_bottomMargin
   *    ratioFloatPixel*yMax-ratioFloatPixel*yMin = viewHeight
   *
   *    ratioFloatPixel = (viewHeight - k_bottomMargin)/yMax
   *    yMin = -k_bottomMargin/ratioFloatPixel = yMax*k_bottomMargin/(k_bottomMargin - viewHeight)
   * */

  m_store->setYMin(m_store->yMax()*(float)Store::k_bottomMargin/((float)Store::k_bottomMargin - m_view.dataViewAtIndex(series)->bounds().height()));
}

void HistogramController::initBarParameters() {
  assert(selectedSeriesIndex() >= 0 && m_store->sumOfOccurrences(selectedSeriesIndex()) > 0);
  float minValue = FLT_MAX;
  float maxValue = -FLT_MAX;
  for (int i = 0; i < Store::k_numberOfSeries; i ++) {
    if (!m_store->seriesIsEmpty(i)) {
      minValue = minFloat(minValue, m_store->minValue(i));
      maxValue = maxFloat(maxValue, m_store->maxValue(i));
    }
  }
  maxValue = minValue >= maxValue ? minValue + std::pow(10.0f, std::floor(std::log10(std::fabs(minValue)))-1.0f) : maxValue;
  m_store->setFirstDrawnBarAbscissa(minValue);
  float barWidth = m_store->computeGridUnit(CurveViewRange::Axis::X, maxValue - minValue);
  if (barWidth <= 0.0f) {
    barWidth = 1.0f;
  }
  m_store->setBarWidth(barWidth);
}

void HistogramController::initBarSelection() {
  assert(selectedSeriesIndex() >= 0 && m_store->sumOfOccurrences(selectedSeriesIndex()) > 0);
  *m_selectedBarIndex = 0;
  while ((m_store->heightOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex) == 0 ||
      m_store->startOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex) < m_store->firstDrawnBarAbscissa()) && *m_selectedBarIndex < m_store->numberOfBars(selectedSeriesIndex())) {
    *m_selectedBarIndex = *m_selectedBarIndex+1;
  }
  if (*m_selectedBarIndex >= m_store->numberOfBars(selectedSeriesIndex())) {
    /* No bar is after m_firstDrawnBarAbscissa, so we select the first bar */
    *m_selectedBarIndex = 0;
    while (m_store->heightOfBarAtIndex(selectedSeriesIndex(), *m_selectedBarIndex) == 0 && *m_selectedBarIndex < m_store->numberOfBars(selectedSeriesIndex())) {
      *m_selectedBarIndex = *m_selectedBarIndex+1;
    }
  }
  m_store->scrollToSelectedBarIndex(selectedSeriesIndex(), *m_selectedBarIndex);
}

}
