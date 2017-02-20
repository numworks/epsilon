#include "histogram_controller.h"
#include "../apps_container.h"
#include "app.h"
#include <assert.h>
#include <math.h>
#include <float.h>

using namespace Poincare;
using namespace Shared;

namespace Statistics {

HistogramController::HistogramController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_bannerView(HistogramBannerView()),
  m_view(HistogramView(store, &m_bannerView)),
  m_settingButton(Button(this, "Reglages de l'histogramme", Invocation([](void * context, void * sender) {
    HistogramController * histogramController = (HistogramController *) context;
    StackViewController * stack = ((StackViewController *)histogramController->stackController());
    stack->push(histogramController->histogramParameterController());
  }, this))),
  m_store(store),
  m_cursor(CurveViewCursor()),
  m_histogramParameterController(nullptr, store)
{
}

const char * HistogramController::title() const {
  return "Histogramme";
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
      headerViewController()->setSelectedButton(-1);
      m_view.selectMainView(true);
      reloadBannerView();
      m_view.reload();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    if (!m_view.isMainViewSelected()) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    m_view.selectMainView(false);
    headerViewController()->setSelectedButton(0);
    return true;
  }
  if (m_view.isMainViewSelected() && (event == Ion::Events::Left || event == Ion::Events::Right)) {
    reloadBannerView();
    int direction = event == Ion::Events::Left ? -1 : 1;
    if (moveSelection(direction)) {
      m_view.reload();
    }
    return true;
  }
  return false;
}

void HistogramController::didBecomeFirstResponder() {
  uint32_t storeChecksum = m_store->storeChecksum();
  if (m_storeVersion != storeChecksum) {
    m_storeVersion = storeChecksum;
    initBarParameters();
  }
  uint32_t barChecksum = m_store->barChecksum();
  if (m_barVersion != barChecksum) {
    m_barVersion = barChecksum;
    initRangeParameters();
  }
  uint32_t rangeChecksum = m_store->rangeChecksum();
  if (m_rangeVersion != rangeChecksum) {
    m_rangeVersion = rangeChecksum;
    initBarSelection();
  }
  m_view.setHighlight(m_store->startOfBarAtIndex(m_selectedBarIndex), m_store->endOfBarAtIndex(m_selectedBarIndex));
  headerViewController()->setSelectedButton(-1);
  m_view.selectMainView(true);
  reloadBannerView();
  m_view.reload();
}

int HistogramController::numberOfButtons() const {
  if (isEmpty()) {
    return 0;
  }
  return 1;
}
Button * HistogramController::buttonAtIndex(int index) {
  return &m_settingButton;
}

bool HistogramController::isEmpty() const {
  if (m_store->sumOfColumn(1) == 0) {
    return true;
  }
  return false;
}

const char * HistogramController::emptyMessage() {
  return "Aucune donnee a tracer";
}

Responder * HistogramController::defaultController() {
  return tabController();
}

void HistogramController::viewWillAppear() {
  reloadBannerView();
  m_view.reload();
}

Responder * HistogramController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

void HistogramController::reloadBannerView() {
  AppsContainer * container = ((App *)app())->container();
  char buffer[k_maxNumberOfCharacters+ Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)*2];
  const char * legend = "Interval [";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float lowerBound = m_store->startOfBarAtIndex(m_selectedBarIndex);
  int lowerBoundNumberOfChar = Complex::convertFloatToText(lowerBound, buffer+legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, container->preferences()->displayMode());
  buffer[legendLength+lowerBoundNumberOfChar] = ';';
  float upperBound = m_store->endOfBarAtIndex(m_selectedBarIndex);
  int upperBoundNumberOfChar = Complex::convertFloatToText(upperBound, buffer+legendLength+lowerBoundNumberOfChar+1, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, container->preferences()->displayMode());
  buffer[legendLength+lowerBoundNumberOfChar+upperBoundNumberOfChar+1] = '[';
  buffer[legendLength+lowerBoundNumberOfChar+upperBoundNumberOfChar+2] = 0;
  m_bannerView.setLegendAtIndex(buffer, 0);

  legend = "Effectif: ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float size = m_store->heightOfBarAtIndex(m_selectedBarIndex);
  Complex::convertFloatToText(size, buffer+legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, container->preferences()->displayMode());
  m_bannerView.setLegendAtIndex(buffer, 1);

  legend = "Frequence: ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  float frequency = size/m_store->sumOfColumn(1);
  Complex::convertFloatToText(frequency, buffer+legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, container->preferences()->displayMode());
  m_bannerView.setLegendAtIndex(buffer, 2);
}

bool HistogramController::moveSelection(int deltaIndex) {
  int newSelectedBarIndex = m_selectedBarIndex;
  if (deltaIndex > 0) {
    do {
      newSelectedBarIndex++;
    } while (m_store->heightOfBarAtIndex(newSelectedBarIndex) == 0 && newSelectedBarIndex < m_store->numberOfBars());
  } else {
    do {
      newSelectedBarIndex--;
    } while (m_store->heightOfBarAtIndex(newSelectedBarIndex) == 0 && newSelectedBarIndex >= 0);
  }
  if (newSelectedBarIndex >= 0 && newSelectedBarIndex < m_store->numberOfBars()) {
    m_selectedBarIndex = newSelectedBarIndex;
    m_view.setHighlight(m_store->startOfBarAtIndex(m_selectedBarIndex), m_store->endOfBarAtIndex(m_selectedBarIndex));
    return m_store->scrollToSelectedBarIndex(m_selectedBarIndex);
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
  m_store->setYMin(-Store::k_displayBottomMarginRatio*yMax);
  m_store->setYMax(yMax*(1.0f+Store::k_displayTopMarginRatio));
}

void HistogramController::initBarParameters() {
  float min = m_store->minValue();
  float max = m_store->maxValue();
  m_store->setFirstDrawnBarAbscissa(min);
  float barWidth = m_store->computeGridUnit(CurveViewRange::Axis::X, min, max);
  if (barWidth <= 0.0f) {
    barWidth = 1.0f;
  }
  m_store->setBarWidth(barWidth);
}

void HistogramController::initBarSelection() {
  m_selectedBarIndex = 0;
  while ((m_store->heightOfBarAtIndex(m_selectedBarIndex) == 0 &&
      m_selectedBarIndex < m_store->numberOfBars()) ||
      m_store->startOfBarAtIndex(m_selectedBarIndex) < m_store->firstDrawnBarAbscissa()) {
    m_selectedBarIndex++;
  }
  if (m_selectedBarIndex >= m_store->numberOfBars()) {
    /* No bar is after m_firstDrawnBarAbscissa, so we select the first bar */
    m_selectedBarIndex = 0;
    while (m_store->heightOfBarAtIndex(m_selectedBarIndex) == 0 && m_selectedBarIndex < m_store->numberOfBars()) {
      m_selectedBarIndex++;
    }
  }
  m_store->scrollToSelectedBarIndex(m_selectedBarIndex);
}

}
