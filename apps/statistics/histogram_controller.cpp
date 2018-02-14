#include "histogram_controller.h"
#include "../apps_container.h"
#include "app.h"
#include <cmath>
#include <assert.h>
#include <float.h>

using namespace Poincare;
using namespace Shared;

namespace Statistics {

HistogramController::HistogramController(Responder * parentResponder, ButtonRowController * header, Store * store, uint32_t * storeVersion, uint32_t * barVersion, uint32_t * rangeVersion, int * selectedBarIndex) :
  ViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_bannerView(),
  m_view(store, &m_bannerView),
  m_settingButton(this, I18n::Message::HistogramSet, Invocation([](void * context, void * sender) {
    HistogramController * histogramController = (HistogramController *) context;
    StackViewController * stack = ((StackViewController *)histogramController->stackController());
    stack->push(histogramController->histogramParameterController());
  }, this)),
  m_store(store),
  m_storeVersion(storeVersion),
  m_barVersion(barVersion),
  m_rangeVersion(rangeVersion),
  m_selectedBarIndex(selectedBarIndex),
  m_histogramParameterController(nullptr, store)
{
}

const char * HistogramController::title() {
  return I18n::translate(I18n::Message::HistogramTab);
}

View * HistogramController::view() {
  return &m_view;
}

StackViewController * HistogramController::stackController() {
  StackViewController * stack = (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
  return stack;
}

HistogramParameterController * HistogramController::histogramParameterController() {
  return &m_histogramParameterController;
}

bool HistogramController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    if (!m_view.isMainViewSelected()) {
      header()->setSelectedButton(-1);
      m_view.selectMainView(true);
      reloadBannerView();
      m_view.reload();
      app()->setFirstResponder(this);
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    if (!m_view.isMainViewSelected()) {
      header()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    m_view.selectMainView(false);
    header()->setSelectedButton(0);
    return true;
  }
  if (m_view.isMainViewSelected() && (event == Ion::Events::Left || event == Ion::Events::Right)) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    if (moveSelection(direction)) {
      reloadBannerView();
      m_view.reload();
    }
    return true;
  }
  return false;
}

void HistogramController::didBecomeFirstResponder() {
  uint32_t storeChecksum = m_store->storeChecksum();
  if (*m_storeVersion != storeChecksum) {
    *m_storeVersion = storeChecksum;
    initBarParameters();
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
  if (!m_view.isMainViewSelected()) {
    header()->setSelectedButton(0);
  } else {
    m_view.setHighlight(m_store->startOfBarAtIndex(*m_selectedBarIndex), m_store->endOfBarAtIndex(*m_selectedBarIndex));
  }
}

int HistogramController::numberOfButtons(ButtonRowController::Position) const {
  if (isEmpty()) {
    return 0;
  }
  return 1;
}
Button * HistogramController::buttonAtIndex(int index, ButtonRowController::Position) const {
  return (Button *)&m_settingButton;
}

bool HistogramController::isEmpty() const {
  if (m_store->sumOfColumn(1) == 0) {
    return true;
  }
  return false;
}

I18n::Message HistogramController::emptyMessage() {
  return I18n::Message::NoDataToPlot;
}

Responder * HistogramController::defaultController() {
  return tabController();
}

void HistogramController::viewWillAppear() {
  if (!m_view.isMainViewSelected()) {
    m_view.selectMainView(true);
    header()->setSelectedButton(-1);
  }
  reloadBannerView();
  m_view.reload();
}

void HistogramController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == nullptr || nextFirstResponder == tabController()) {
    m_view.selectMainView(false);
    header()->setSelectedButton(-1);
    m_view.reload();
  }
}

Responder * HistogramController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

void HistogramController::reloadBannerView() {
  char buffer[k_maxNumberOfCharacters+ PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)*2];
  int numberOfChar = 0;
  const char * legend = " [";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  double lowerBound = m_store->startOfBarAtIndex(*m_selectedBarIndex);
  numberOfChar += PrintFloat::convertFloatToText<double>(lowerBound, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  buffer[numberOfChar++] = ';';
  double upperBound = m_store->endOfBarAtIndex(*m_selectedBarIndex);
  numberOfChar += PrintFloat::convertFloatToText<double>(upperBound, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  buffer[numberOfChar++] = '[';
  legend = "                                  ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxIntervalLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 1);

  numberOfChar = 0;
  legend = ": ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  double size = m_store->heightOfBarAtIndex(*m_selectedBarIndex);
  numberOfChar += PrintFloat::convertFloatToText<double>(size, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "                            ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 3);

  numberOfChar = 0;
  legend = ": ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  double frequency = size/m_store->sumOfColumn(1);
  numberOfChar += PrintFloat::convertFloatToText<double>(frequency, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "                            ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 5);
}

bool HistogramController::moveSelection(int deltaIndex) {
  int newSelectedBarIndex = *m_selectedBarIndex;
  if (deltaIndex > 0) {
    do {
      newSelectedBarIndex++;
    } while (m_store->heightOfBarAtIndex(newSelectedBarIndex) == 0 && newSelectedBarIndex < m_store->numberOfBars());
  } else {
    do {
      newSelectedBarIndex--;
    } while (m_store->heightOfBarAtIndex(newSelectedBarIndex) == 0 && newSelectedBarIndex >= 0);
  }
  if (newSelectedBarIndex >= 0 && newSelectedBarIndex < m_store->numberOfBars() && *m_selectedBarIndex != newSelectedBarIndex) {
    *m_selectedBarIndex = newSelectedBarIndex;
    m_view.setHighlight(m_store->startOfBarAtIndex(*m_selectedBarIndex), m_store->endOfBarAtIndex(*m_selectedBarIndex));
    m_store->scrollToSelectedBarIndex(*m_selectedBarIndex);
    return true;
  }
  return false;
}

void HistogramController::initRangeParameters() {
  float min = m_store->firstDrawnBarAbscissa();
  float max = m_store->maxValue();
  float barWidth = m_store->barWidth();
  float xMin = min;
  float xMax = max + barWidth;
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
  float yMax = -FLT_MAX;
  for (int index = 0; index < m_store->numberOfBars(); index++) {
    float size = m_store->heightOfBarAtIndex(index);
    if (size > yMax) {
      yMax = size;
    }
  }
  yMax = yMax/m_store->sumOfColumn(1);
  yMax = yMax < 0 ? 1 : yMax;
  m_store->setYMin(-Store::k_displayBottomMarginRatio*yMax);
  m_store->setYMax(yMax*(1.0f+Store::k_displayTopMarginRatio));
}

void HistogramController::initBarParameters() {
  float min = m_store->minValue();
  float max = m_store->maxValue();
  max = min >= max ? min + std::pow(10.0f, std::floor(std::log10(std::fabs(min)))-1.0f) : max;
  m_store->setFirstDrawnBarAbscissa(min);
  float barWidth = m_store->computeGridUnit(CurveViewRange::Axis::X, min, max);
  if (barWidth <= 0.0f) {
    barWidth = 1.0f;
  }
  m_store->setBarWidth(barWidth);
}

void HistogramController::initBarSelection() {
  *m_selectedBarIndex = 0;
  while ((m_store->heightOfBarAtIndex(*m_selectedBarIndex) == 0 ||
      m_store->startOfBarAtIndex(*m_selectedBarIndex) < m_store->firstDrawnBarAbscissa()) &&      *m_selectedBarIndex < m_store->numberOfBars()) {
    *m_selectedBarIndex = *m_selectedBarIndex+1;
  }
  if (*m_selectedBarIndex >= m_store->numberOfBars()) {
    /* No bar is after m_firstDrawnBarAbscissa, so we select the first bar */
    *m_selectedBarIndex = 0;
    while (m_store->heightOfBarAtIndex(*m_selectedBarIndex) == 0 && *m_selectedBarIndex < m_store->numberOfBars()) {
      *m_selectedBarIndex = *m_selectedBarIndex+1;
    }
  }
  m_store->scrollToSelectedBarIndex(*m_selectedBarIndex);
}

}
