#include "graph_controller.h"
#include "../apps_container.h"
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Regression {

GraphController::GraphController(Responder * parentResponder, ButtonRowController * header, Store * store, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion, int * selectedDotIndex) :
  InteractiveCurveViewController(parentResponder, header, store, &m_view, cursor, modelVersion, rangeVersion),
  m_crossCursorView(),
  m_roundCursorView(Palette::YellowDark),
  m_bannerView(),
  m_view(store, m_cursor, &m_bannerView, &m_crossCursorView),
  m_store(store),
  m_initialisationParameterController(this, m_store),
  m_predictionParameterController(this, m_store, m_cursor, this),
  m_selectedDotIndex(selectedDotIndex)
{
  m_store->setCursor(m_cursor);
}

ViewController * GraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
}

bool GraphController::isEmpty() const {
  if (m_store->numberOfPairs() < 2 || std::isinf(m_store->slope()) || std::isnan(m_store->slope())) {
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

void GraphController::viewWillAppear() {
  InteractiveCurveViewController::viewWillAppear();
  m_view.setCursorView(*m_selectedDotIndex >= 0 ? static_cast<View *>(&m_crossCursorView): static_cast<View *>(&m_roundCursorView));
}

void GraphController::selectRegressionCurve() {
  *m_selectedDotIndex = -1;
  m_view.setCursorView(&m_roundCursorView);
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
  m_bannerView.setMessageAtIndex(I18n::Message::RegressionFormula, 3);

  char buffer[k_maxNumberOfCharacters + PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  int numberOfChar = 0;
  const char * legend = " P(";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  if (*m_selectedDotIndex == m_store->numberOfPairs()) {
    legend = I18n::translate(I18n::Message::MeanDot);
    legendLength = strlen(legend);
    strlcpy(buffer+numberOfChar, legend, legendLength+1);
    numberOfChar += legendLength;
  } else if (*m_selectedDotIndex < 0) {
    legend = I18n::translate(I18n::Message::Reg);
    legendLength = strlen(legend);
    strlcpy(buffer+numberOfChar, legend, legendLength+1);
    numberOfChar += legendLength;
  } else {
    numberOfChar += PrintFloat::convertFloatToText<float>(std::round((float)*m_selectedDotIndex+1.0f), buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits), Constant::ShortNumberOfSignificantDigits, PrintFloat::Mode::Decimal);
  }
  legend = ")  ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 0);

  numberOfChar = 0;
  legend = "x=";
  double x = m_cursor->x();
  // Display a specific legend if the mean dot is selected
  if (*m_selectedDotIndex == m_store->numberOfPairs()) {
    constexpr static char legX[] = {Ion::Charset::XBar, '=', 0};
    legend = legX;
    x = m_store->meanOfColumn(0);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += PrintFloat::convertFloatToText<double>(x, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  legend = "                  ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 1);

  numberOfChar = 0;
  legend = "y=";
  double y = m_cursor->y();
  if (*m_selectedDotIndex == m_store->numberOfPairs()) {
    constexpr static char legY[] = {Ion::Charset::YBar, '=', 0};
    legend = legY;
    y = m_store->meanOfColumn(1);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += PrintFloat::convertFloatToText<double>(y, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  legend = "                  ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 2);

  numberOfChar = 0;
  legend = " a=";
  double slope = m_store->slope();
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += PrintFloat::convertFloatToText<double>(slope, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "                  ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 4);

  numberOfChar = 0;
  legend = " b=";
  double yIntercept = m_store->yIntercept();
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += PrintFloat::convertFloatToText<double>(yIntercept, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "                  ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 5);

  numberOfChar = 0;
  legend = "           r=";
  double r = m_store->correlationCoefficient();
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += PrintFloat::convertFloatToText<double>(r, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "                  ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength+10] = 0;
  m_bannerView.setLegendAtIndex(buffer, 6);

  numberOfChar = 0;
  legend = " r2=";
  double r2 = m_store->squaredCorrelationCoefficient();
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += PrintFloat::convertFloatToText<double>(r2, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "                  ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 7);
}

void GraphController::initRangeParameters() {
  m_store->setDefault();
}

void GraphController::initCursorParameters() {
  double x = m_store->meanOfColumn(0);
  double y = m_store->meanOfColumn(1);
  m_cursor->moveTo(x, y);
  m_store->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  *m_selectedDotIndex = m_store->numberOfPairs();
}

bool GraphController::moveCursorHorizontally(int direction) {
  if (*m_selectedDotIndex >= 0) {
    int dotSelected = m_store->nextDot(direction, *m_selectedDotIndex);
    if (dotSelected >= 0 && dotSelected < m_store->numberOfPairs()) {
      *m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->get(0, *m_selectedDotIndex), m_store->get(1, *m_selectedDotIndex));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    }
    if (dotSelected == m_store->numberOfPairs()) {
      *m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->meanOfColumn(0), m_store->meanOfColumn(1));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    }
    return false;
  }
  double x = direction > 0 ? m_cursor->x() + m_store->xGridUnit()/k_numberOfCursorStepsInGradUnit :
  m_cursor->x() - m_store->xGridUnit()/k_numberOfCursorStepsInGradUnit;
  double y = m_store->yValueForXValue(x);
  m_cursor->moveTo(x, y);
  m_store->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

bool GraphController::moveCursorVertically(int direction) {
  double yRegressionCurve = m_store->yValueForXValue(m_cursor->x());
  if (*m_selectedDotIndex >= 0) {
    if ((yRegressionCurve - m_cursor->y() > 0) == (direction > 0)) {
      selectRegressionCurve();
      m_cursor->moveTo(m_cursor->x(), yRegressionCurve);
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    } else {
      return false;
    }
  } else {
    int dotSelected = m_store->closestVerticalDot(direction, m_cursor->x());
    if (dotSelected >= 0 && dotSelected <= m_store->numberOfPairs()) {
      m_view.setCursorView(&m_crossCursorView);
      if (dotSelected == m_store->numberOfPairs()) {
        *m_selectedDotIndex = dotSelected;
        m_cursor->moveTo(m_store->meanOfColumn(0), m_store->meanOfColumn(1));
        m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
        return true;
      }
      *m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->get(0, *m_selectedDotIndex), m_store->get(1, *m_selectedDotIndex));
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
