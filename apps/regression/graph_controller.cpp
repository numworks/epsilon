#include "graph_controller.h"
#include "../apps_container.h"

using namespace Poincare;
using namespace Shared;

namespace Regression {

GraphController::GraphController(Responder * parentResponder, ButtonRowController * header, Store * store, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion) :
  InteractiveCurveViewController(parentResponder, header, store, &m_view, cursor, modelVersion, rangeVersion),
  m_bannerView(),
  m_view(store, m_cursor, &m_bannerView, &m_cursorView),
  m_store(store),
  m_initialisationParameterController(this, m_store),
  m_predictionParameterController(this, m_store, m_cursor, this),
  m_selectedDotIndex(-1)
{
  m_store->setCursor(m_cursor);
}

ViewController * GraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
}

bool GraphController::isEmpty() const {
  if (m_store->numberOfPairs() < 2 || isinf(m_store->slope()) || isnan(m_store->slope())) {
    return true;
  }
  return false;
}

I18n::Message GraphController::emptyMessage() {
  if (m_store->numberOfPairs() == 0) {
    return I18n::Message::NoDataToPlot;
  }
  return I18n::Message::NoEnoughDataForRegression;
}

void GraphController::selectRegressionCurve() {
  m_selectedDotIndex = -1;
}

BannerView * GraphController::bannerView() {
  return &m_bannerView;
}

CurveView * GraphController::curveView() {
  return &m_view;
}

InteractiveCurveViewRange * GraphController::interactiveCurveViewRange() {
  return m_store;
}

bool GraphController::handleEnter() {
  stackController()->push(&m_predictionParameterController);
  return true;
}

void GraphController::reloadBannerView() {
  m_bannerView.setMessageAtIndex(I18n::Message::RegressionFormula, 0);
  char buffer[k_maxNumberOfCharacters + Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  int numberOfChar = 0;
  const char * legend = " a=";
  float slope = m_store->slope();
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += Complex::convertFloatToText(slope, buffer+numberOfChar, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "             ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 1);

  numberOfChar = 0;
  legend = " b=";
  float yIntercept = m_store->yIntercept();
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += Complex::convertFloatToText(yIntercept, buffer+numberOfChar, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "             ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 2);

  numberOfChar = 0;
  legend = " x=";
  float x = m_cursor->x();
  // Display a specific legend if the mean dot is selected
  if (m_selectedDotIndex == m_store->numberOfPairs()) {
    constexpr static char legX[] = {Ion::Charset::XBar, '=', 0};
    legend = legX;
    x = m_store->meanOfColumn(0);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += Complex::convertFloatToText(x, buffer+numberOfChar, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "             ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 3);

  numberOfChar = 0;
  legend = " y=";
  float y = m_cursor->y();
  if (m_selectedDotIndex == m_store->numberOfPairs()) {
    constexpr static char legY[] = {Ion::Charset::YBar, '=', 0};
    legend = legY;
    y = m_store->meanOfColumn(1);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += Complex::convertFloatToText(y, buffer+numberOfChar, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "                ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 4);


  numberOfChar = 0;
  legend = " r=";
  float r = m_store->correlationCoefficient();
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += Complex::convertFloatToText(r, buffer+numberOfChar, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "             ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 5);

  numberOfChar = 0;
  legend = " r^2=";
  float r2 = m_store->squaredCorrelationCoefficient();
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += Complex::convertFloatToText(r2, buffer+numberOfChar, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "                ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 6);
}

void GraphController::initRangeParameters() {
  m_store->setDefault();
}

void GraphController::initCursorParameters() {
  float x = (m_store->xMin() + m_store->xMax())/2.0f;
  float y = m_store->yValueForXValue(x);
  m_cursor->moveTo(x, y);
  m_store->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  m_selectedDotIndex = -1;
}

bool GraphController::moveCursorHorizontally(int direction) {
  if (m_selectedDotIndex >= 0) {
    int dotSelected = m_store->nextDot(direction, m_selectedDotIndex);
    if (dotSelected >= 0 && dotSelected < m_store->numberOfPairs()) {
      m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->get(0, m_selectedDotIndex), m_store->get(1, m_selectedDotIndex));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    }
    if (dotSelected == m_store->numberOfPairs()) {
      m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->meanOfColumn(0), m_store->meanOfColumn(1));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    }
    return false;
  }
  float x = direction > 0 ? m_cursor->x() + m_store->xGridUnit()/k_numberOfCursorStepsInGradUnit :
  m_cursor->x() - m_store->xGridUnit()/k_numberOfCursorStepsInGradUnit;
  float y = m_store->yValueForXValue(x);
  m_cursor->moveTo(x, y);
  m_store->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

bool GraphController::moveCursorVertically(int direction) {
  float yRegressionCurve = m_store->yValueForXValue(m_cursor->x());
  if (m_selectedDotIndex >= 0) {
    if ((yRegressionCurve - m_cursor->y() > 0) == (direction > 0)) {
      m_selectedDotIndex = -1;
      m_cursor->moveTo(m_cursor->x(), yRegressionCurve);
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    } else {
      return false;
    }
  } else {
    int dotSelected = m_store->closestVerticalDot(direction, m_cursor->x());
    if (dotSelected >= 0 && dotSelected < m_store->numberOfPairs()) {
      m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->get(0, m_selectedDotIndex), m_store->get(1, m_selectedDotIndex));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    }
    if (dotSelected == m_store->numberOfPairs()) {
      m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->meanOfColumn(0), m_store->meanOfColumn(1));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    }
    return false;
  }
}

uint32_t GraphController::modelVersion() {
  return m_store->storeChecksum();
}

uint32_t GraphController::rangeVersion() {
  return m_store->rangeChecksum();
}

bool GraphController::isCursorVisible() {
  return interactiveCurveViewRange()->isCursorVisible(k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

}
