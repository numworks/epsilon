#include "graph_controller.h"
#include "../app.h"
#include "../../apps_container.h"
#include <assert.h>
#include <math.h>
#include <float.h>

using namespace Poincare;
using namespace Shared;

namespace Graph {

GraphController::GraphController(Responder * parentResponder, CartesianFunctionStore * functionStore, HeaderViewController * header) :
  InteractiveCurveViewController(parentResponder, header, &m_graphRange, &m_view),
  m_bannerView(BannerView()),
  m_view(GraphView(functionStore, &m_graphRange, &m_cursor, &m_bannerView, &m_cursorView)),
  m_graphRange(InteractiveCurveViewRange(&m_cursor, this)),
  m_initialisationParameterController(InitialisationParameterController(this, &m_graphRange)),
  m_curveParameterController(CurveParameterController(&m_graphRange, &m_bannerView, &m_cursor)),
  m_functionStore(functionStore),
  m_indexFunctionSelectedByCursor(0)
{
}

bool GraphController::isEmpty() const {
  if (m_functionStore->numberOfActiveFunctions() == 0) {
    return true;
  }
  return false;
}

const char * GraphController::emptyMessage() {
  if (m_functionStore->numberOfDefinedFunctions() == 0) {
    return "Aucune fonction";
  }
  return "Aucune fonction selectionnee";
}

ViewController * GraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
}

void GraphController::viewWillAppear() {
  if (m_view.context() == nullptr) {
    App * graphApp = (Graph::App *)app();
    m_view.setContext(graphApp->localContext());
  }
  Expression::AngleUnit newAngleUnitVersion = Preferences::sharedPreferences()->angleUnit();
  if (m_angleUnitVersion != newAngleUnitVersion) {
    m_angleUnitVersion = newAngleUnitVersion;
    initCursorParameters();
  }
  InteractiveCurveViewController::viewWillAppear();
}

bool GraphController::didChangeRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  if (!m_graphRange.yAuto()) {
    return false;
  }
  App * graphApp = (Graph::App *)app();
  if (m_functionStore->numberOfActiveFunctions() <= 0) {
    return false;
  }
  float min = FLT_MAX;
  float max = -FLT_MAX;
  float xMin = m_graphRange.xMin();
  float xMax = m_graphRange.xMax();
  float step = (xMax - xMin)/Ion::Display::Width;
  for (int i=0; i<m_functionStore->numberOfActiveFunctions(); i++) {
    CartesianFunction * f = m_functionStore->activeFunctionAtIndex(i);
    float y = 0.0f;
    for (int i = 0; i <= Ion::Display::Width; i++) {
      float x = xMin + i*step;
      y = f->evaluateAtAbscissa(x, graphApp->localContext());
      if (!isnan(y) && !isinf(y)) {
        min = min < y ? min : y;
        max = max > y ? max : y;
      }
    }
  }
  float range = max - min;
  if (m_graphRange.yMin() == min-k_displayBottomMarginRatio*range
      && m_graphRange.yMax() == max+k_displayTopMarginRatio*range) {
    return false;
  }
  if (min == max) {
    min = min - 1;
    max = max + 1;
  }
  if (min == FLT_MAX && max == -FLT_MAX) {
    min = -1.0f;
    max = 1.0f;
  }
  if (min == FLT_MAX) {
    min = max-1.0f;
  }
   if (max == -FLT_MAX) {
    max = min+1.0f;
  }
  m_graphRange.setYMin(min-k_displayBottomMarginRatio*range);
  m_graphRange.setYMax(max+k_displayTopMarginRatio*range);
  if (isinf(m_graphRange.xMin())) {
    m_graphRange.setYMin(-FLT_MAX);
  }
  if (isinf(m_graphRange.xMax())) {
    m_graphRange.setYMax(FLT_MAX);
  }
  return true;
}

BannerView * GraphController::bannerView() {
  return &m_bannerView;
}

bool GraphController::handleEnter() {
  CartesianFunction * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  m_curveParameterController.setFunction(f);
  StackViewController * stack = stackController();
  stack->push(&m_curveParameterController);
  return true;
}

void GraphController::reloadBannerView() {
  char buffer[k_maxNumberOfCharacters+Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * legend = "x = ";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  Complex::convertFloatToText(m_cursor.x(), buffer+ legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  m_bannerView.setLegendAtIndex(buffer, 0);

  legend = "00(x) = ";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  if (m_functionStore->numberOfActiveFunctions() == 0) {
    return;
  }
  CartesianFunction * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  buffer[1] = f->name()[0];
  Complex::convertFloatToText(m_cursor.y(), buffer+legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  m_bannerView.setLegendAtIndex(buffer+1, 1);

  if (m_bannerView.displayDerivative()) {
    buffer[0] = f->name()[0];
    buffer[1] = '\'';
    App * graphApp = (Graph::App *)app();
    float y = f->approximateDerivative(m_cursor.x(), graphApp->localContext());
    Complex::convertFloatToText(y, buffer + legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    m_bannerView.setLegendAtIndex(buffer, 2);
  }
}

void GraphController::initRangeParameters() {
  if (didChangeRange(&m_graphRange)) {
    initCursorParameters();
  }
}

void GraphController::initCursorParameters() {
  float x = (m_graphRange.xMin()+m_graphRange.xMax())/2.0f;
  m_indexFunctionSelectedByCursor = 0;
  App * graphApp = (Graph::App *)app();
  int functionIndex = 0;
  float y = 0;
  do {
    CartesianFunction * firstFunction = m_functionStore->activeFunctionAtIndex(functionIndex++);
    y = firstFunction->evaluateAtAbscissa(x, graphApp->localContext());
  } while (isnan(y) && functionIndex < m_functionStore->numberOfActiveFunctions());
  m_cursor.moveTo(x, y);
  m_graphRange.panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

bool GraphController::moveCursorHorizontally(int direction) {
  float xCursorPosition = m_cursor.x();
  float x = direction > 0 ? xCursorPosition + m_graphRange.xGridUnit()/k_numberOfCursorStepsInGradUnit :
    xCursorPosition -  m_graphRange.xGridUnit()/k_numberOfCursorStepsInGradUnit;
  CartesianFunction * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  App * graphApp = (Graph::App *)app();
  float y = f->evaluateAtAbscissa(x, graphApp->localContext());
  m_cursor.moveTo(x, y);
  m_graphRange.panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

bool GraphController::moveCursorVertically(int direction) {
  CartesianFunction * actualFunction = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  App * graphApp = (Graph::App *)app();
  float y = actualFunction->evaluateAtAbscissa(m_cursor.x(), graphApp->localContext());
  CartesianFunction * nextFunction = actualFunction;
  float nextY = direction > 0 ? FLT_MAX : -FLT_MAX;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    CartesianFunction * f = m_functionStore->activeFunctionAtIndex(i);
    float newY = f->evaluateAtAbscissa(m_cursor.x(), graphApp->localContext());
    bool isNextFunction = direction > 0 ? (newY > y && newY < nextY) : (newY < y && newY > nextY);
    if (isNextFunction) {
      m_indexFunctionSelectedByCursor = i;
      nextY = newY;
      nextFunction = f;
    }
  }
  if (nextFunction == actualFunction) {
    return false;
  }
  m_cursor.moveTo(m_cursor.x(), nextY);
  m_graphRange.panToMakePointVisible(m_cursor.x(), m_cursor.y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

uint32_t GraphController::modelVersion() {
  return m_functionStore->storeChecksum();
}

uint32_t GraphController::rangeVersion() {
  return m_graphRange.rangeChecksum();
}

InteractiveCurveViewRange * GraphController::interactiveCurveViewRange() {
  return &m_graphRange;
}

CurveView * GraphController::curveView() {
  return &m_view;
}

}
