#include "histogram_controller.h"
#include "../apps_container.h"
#include "app.h"
#include <cmath>
#include <assert.h>
#include <float.h>

using namespace Poincare;
using namespace Shared;

namespace Statistics {

static inline float min(float x, float y) { return (x<y ? x : y); }
static inline float max(float x, float y) { return (x>y ? x : y); }

HistogramController::HistogramController(Responder * parentResponder, ButtonRowController * header, Store * store, uint32_t * storeVersion, uint32_t * barVersion, uint32_t * rangeVersion, int * selectedBarIndex) :
  ViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_store(store),
  m_view(this, store),
  m_storeVersion(storeVersion),
  m_barVersion(barVersion),
  m_rangeVersion(rangeVersion),
  m_selectedSeries(-1),
  m_selectedBarIndex(selectedBarIndex),
  m_histogramParameterController(nullptr, store)
{
}

StackViewController * HistogramController::stackController() {
  StackViewController * stack = (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
  return stack;
}

void HistogramController::setCurrentDrawnSeries(int series) {
  initYRangeParameters(series);
}

bool HistogramController::isEmpty() const {
  return m_store->isEmpty();
}

I18n::Message HistogramController::emptyMessage() {
  return I18n::Message::NoDataToPlot;
}

Responder * HistogramController::defaultController() {
  return tabController();
}

const char * HistogramController::title() {
  return I18n::translate(I18n::Message::HistogramTab);
}

void HistogramController::viewWillAppear() {
  m_view.setDisplayBanner(true);
  if (m_selectedSeries < 0) {
    m_selectedSeries = m_view.seriesOfSubviewAtIndex(0);
    m_view.selectHistogram(m_selectedSeries);
  }
  reloadBannerView();
  m_view.reload();
}

bool HistogramController::handleEvent(Ion::Events::Event event) {
  assert(m_selectedSeries >= 0);
  if (event == Ion::Events::Down) {
    int currentSelectedSubview = m_view.indexOfSubviewAtSeries(m_selectedSeries);
    if (currentSelectedSubview < m_view.numberOfSubviews() - 2) {
      m_view.deselectHistogram(m_selectedSeries);
      m_selectedSeries = m_view.seriesOfSubviewAtIndex(currentSelectedSubview+1);
      *m_selectedBarIndex = 0;
      m_view.selectHistogram(m_selectedSeries);
      reloadBannerView();
      m_view.reload();
      app()->setFirstResponder(this);
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    int currentSelectedSubview = m_view.indexOfSubviewAtSeries(m_selectedSeries);
    if (currentSelectedSubview > 0) {
      m_view.deselectHistogram(m_selectedSeries);
      assert(currentSelectedSubview > 0);
      m_selectedSeries = m_view.seriesOfSubviewAtIndex(currentSelectedSubview-1);
      m_view.selectHistogram(m_selectedSeries);
      *m_selectedBarIndex = 0;
      app()->setFirstResponder(this);
    } else {
      app()->setFirstResponder(tabController());
    }
    reloadBannerView();
    m_view.reload();
    return true;
  }
  if (m_selectedSeries >= 0 && (event == Ion::Events::Left || event == Ion::Events::Right)) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    if (moveSelection(direction)) {
      reloadBannerView();
      m_view.reload();
    }
    return true;
  }
  if (event == Ion::Events::OK) {
    stackController()->push(histogramParameterController());
    return true;
  }
  return false;
}

void HistogramController::didBecomeFirstResponder() {
  m_view.setDisplayBanner(true);
  if (m_selectedSeries < 0 || m_store->sumOfOccurrences(m_selectedSeries) == 0) {
    if (m_selectedSeries >= 0) {
      m_view.deselectHistogram(m_selectedSeries);
    }
    m_selectedSeries = m_view.seriesOfSubviewAtIndex(0);
    m_view.selectHistogram(m_selectedSeries);
    m_view.reload();
  }
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
  m_view.histogramViewAtIndex(m_selectedSeries)->setHighlight(m_store->startOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex), m_store->endOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex));
}

void HistogramController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == nullptr || nextFirstResponder == tabController()) {
    if (m_selectedSeries >= 0) {
      m_view.histogramViewAtIndex(m_selectedSeries)->selectMainView(false);
      m_view.histogramViewAtIndex(m_selectedSeries)->setForceOkDisplay(false);
      m_selectedSeries = -1;
      m_view.setDisplayBanner(false);
    }
    m_view.reload();
  }
}

Responder * HistogramController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

void HistogramController::reloadBannerView() {
  if (m_selectedSeries < 0) {
    return;
  }
  char buffer[k_maxNumberOfCharacters+ PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)*2];
  int numberOfChar = 0;

  // Add Interval Data
  const char * legend = " [";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;

  // Add lower bound
  if (m_selectedSeries >= 0) {
    double lowerBound = m_store->startOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex);
    numberOfChar += PrintFloat::convertFloatToText<double>(lowerBound, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  }

  buffer[numberOfChar++] = ';';

  // Add upper bound
  if (m_selectedSeries >= 0) {
    double upperBound = m_store->endOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex);
    numberOfChar += PrintFloat::convertFloatToText<double>(upperBound, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  }

  buffer[numberOfChar++] = '[';

  // Padding
  for (int i = numberOfChar; i < k_maxIntervalLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxIntervalLegendLength] = 0;
  m_view.bannerView()->setLegendAtIndex(buffer, 1);

  // Add Size Data
  numberOfChar = 0;
  legend = ": ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  double size = 0;
  if (m_selectedSeries >= 0) {
    size = m_store->heightOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex);
    numberOfChar += PrintFloat::convertFloatToText<double>(size, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  }
  // Padding
  for (int i = numberOfChar; i < k_maxLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxLegendLength] = 0;
  m_view.bannerView()->setLegendAtIndex(buffer, 3);

  // Add Frequency Data
  numberOfChar = 0;
  legend = ": ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  if (m_selectedSeries >= 0) {
    double frequency = size/m_store->sumOfOccurrences(m_selectedSeries);
    numberOfChar += PrintFloat::convertFloatToText<double>(frequency, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  }
  // Padding
  for (int i = numberOfChar; i < k_maxLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxLegendLength] = 0;
  m_view.bannerView()->setLegendAtIndex(buffer, 5);
}

bool HistogramController::moveSelection(int deltaIndex) {
  int newSelectedBarIndex = *m_selectedBarIndex;
  do {
    newSelectedBarIndex+=deltaIndex;
  } while (m_store->heightOfBarAtIndex(m_selectedSeries, newSelectedBarIndex) == 0
      && newSelectedBarIndex >= 0
      && newSelectedBarIndex < m_store->numberOfBars(m_selectedSeries));

  if (newSelectedBarIndex >= 0
      && newSelectedBarIndex < m_store->numberOfBars(m_selectedSeries)
      && *m_selectedBarIndex != newSelectedBarIndex)
  {
    *m_selectedBarIndex = newSelectedBarIndex;
    m_view.histogramViewAtIndex(m_selectedSeries)->setHighlight(m_store->startOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex), m_store->endOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex));
    m_store->scrollToSelectedBarIndex(m_selectedSeries, *m_selectedBarIndex);
    return true;
  }
  return false;
}

void HistogramController::initRangeParameters() {
  assert(m_selectedSeries >= 0 && m_store->sumOfOccurrences(m_selectedSeries) > 0);
  float minValue = m_store->firstDrawnBarAbscissa();
  float maxValue = -FLT_MAX;
  for (int i = 0; i < Store::k_numberOfSeries; i ++) {
    if (!m_store->seriesIsEmpty(i)) {
      maxValue = max(maxValue, m_store->maxValue(i));
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

  initYRangeParameters(m_selectedSeries);
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

  m_store->setYMin(m_store->yMax()*(float)Store::k_bottomMargin/((float)Store::k_bottomMargin - m_view.histogramViewAtIndex(series)->bounds().height()));
}

void HistogramController::initBarParameters() {
  assert(m_selectedSeries >= 0 && m_store->sumOfOccurrences(m_selectedSeries) > 0);
  float minValue = FLT_MAX;
  float maxValue = -FLT_MAX;
  for (int i = 0; i < Store::k_numberOfSeries; i ++) {
    if (!m_store->seriesIsEmpty(i)) {
      minValue = min(minValue, m_store->minValue(i));
      maxValue = max(maxValue, m_store->maxValue(i));
    }
  }
  maxValue = minValue >= maxValue ? minValue + std::pow(10.0f, std::floor(std::log10(std::fabs(minValue)))-1.0f) : maxValue;
  m_store->setFirstDrawnBarAbscissa(minValue);
  float barWidth = m_store->computeGridUnit(CurveViewRange::Axis::X, minValue, maxValue);
  if (barWidth <= 0.0f) {
    barWidth = 1.0f;
  }
  m_store->setBarWidth(barWidth);
}

void HistogramController::initBarSelection() {
  assert(m_selectedSeries >= 0 && m_store->sumOfOccurrences(m_selectedSeries) > 0);
  *m_selectedBarIndex = 0;
  while ((m_store->heightOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex) == 0 ||
      m_store->startOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex) < m_store->firstDrawnBarAbscissa()) && *m_selectedBarIndex < m_store->numberOfBars(m_selectedSeries)) {
    *m_selectedBarIndex = *m_selectedBarIndex+1;
  }
  if (*m_selectedBarIndex >= m_store->numberOfBars(m_selectedSeries)) {
    /* No bar is after m_firstDrawnBarAbscissa, so we select the first bar */
    *m_selectedBarIndex = 0;
    while (m_store->heightOfBarAtIndex(m_selectedSeries, *m_selectedBarIndex) == 0 && *m_selectedBarIndex < m_store->numberOfBars(m_selectedSeries)) {
      *m_selectedBarIndex = *m_selectedBarIndex+1;
    }
  }
  m_store->scrollToSelectedBarIndex(m_selectedSeries, *m_selectedBarIndex);
}

}
