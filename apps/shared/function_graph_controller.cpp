#include "function_graph_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>
#include <math.h>
#include <float.h>

using namespace Poincare;

namespace Shared {

FunctionGraphController::FunctionGraphController(Responder * parentResponder, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView) :
  InteractiveCurveViewController(parentResponder, header, interactiveRange, curveView),
  m_indexFunctionSelectedByCursor(0),
  m_initialisationParameterController(InitialisationParameterController(this, interactiveRange))
{
}

bool FunctionGraphController::isEmpty() const {
  if (functionStore()->numberOfActiveFunctions() == 0) {
    return true;
  }
  return false;
}

ViewController * FunctionGraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
}

void FunctionGraphController::viewWillAppear() {
  if (functionGraphView()->context() == nullptr) {
    TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
    functionGraphView()->setContext(myApp->localContext());
  }
  Expression::AngleUnit newAngleUnitVersion = Preferences::sharedPreferences()->angleUnit();
  if (m_angleUnitVersion != newAngleUnitVersion) {
    m_angleUnitVersion = newAngleUnitVersion;
    initCursorParameters();
  }
  InteractiveCurveViewController::viewWillAppear();
}

bool FunctionGraphController::didChangeRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  if (!interactiveCurveViewRange->yAuto()) {
    return false;
  }
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  if (functionStore()->numberOfActiveFunctions() <= 0) {
    return false;
  }
  float min = FLT_MAX;
  float max = -FLT_MAX;
  float xMin = interactiveCurveViewRange->xMin();
  float xMax = interactiveCurveViewRange->xMax();
  float step = (xMax - xMin)/curveView()->resolution();
  for (int i=0; i<functionStore()->numberOfActiveFunctions(); i++) {
    Function * f = functionStore()->activeFunctionAtIndex(i);
    float y = 0.0f;
    for (int i = 0; i <= curveView()->resolution(); i++) {
      float x = xMin + i*step;
      y = f->evaluateAtAbscissa(x, myApp->localContext());
      if (!isnan(y) && !isinf(y)) {
        min = min < y ? min : y;
        max = max > y ? max : y;
      }
    }
  }
  float range = max - min;
  if (max < min) {
    range = 0.0f;
  }
  if (interactiveCurveViewRange->yMin() == min-k_displayBottomMarginRatio*range
      && interactiveCurveViewRange->yMax() == max+k_displayTopMarginRatio*range) {
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
  interactiveCurveViewRange->setYMin(min-k_displayBottomMarginRatio*range);
  interactiveCurveViewRange->setYMax(max+k_displayTopMarginRatio*range);
  if (isinf(interactiveCurveViewRange->xMin())) {
    interactiveCurveViewRange->setYMin(-FLT_MAX);
  }
  if (isinf(interactiveCurveViewRange->xMax())) {
    interactiveCurveViewRange->setYMax(FLT_MAX);
  }
  return true;
}

bool FunctionGraphController::handleEnter() {
  Function * f = functionStore()->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  curveParameterController()->setFunction(f);
  StackViewController * stack = stackController();
  stack->push(curveParameterController());
  return true;
}

void FunctionGraphController::reloadBannerView() {
  char buffer[k_maxNumberOfCharacters+Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * legend = "0=";
  int legendLength = strlen(legend);
  int numberOfChar = 0;
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  buffer[0] = functionStore()->symbol();
  numberOfChar += Complex::convertFloatToText(m_cursor.x(), buffer+numberOfChar, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "   ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  bannerView()->setLegendAtIndex(buffer, 0);

  numberOfChar = 0;
  legend = "0(x)=";
  legendLength = strlen(legend);
  numberOfChar += legendLength;
  strlcpy(buffer, legend, legendLength+1);
  buffer[2] = functionStore()->symbol();
  if (functionStore()->numberOfActiveFunctions() == 0) {
    return;
  }
  Function * f = functionStore()->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  buffer[0] = f->name()[0];
  numberOfChar += Complex::convertFloatToText(m_cursor.y(), buffer+legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  legend = "   ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  bannerView()->setLegendAtIndex(buffer, 1);
}

void FunctionGraphController::initRangeParameters() {
  if (didChangeRange(interactiveCurveViewRange())) {
    initCursorParameters();
  }
}

bool FunctionGraphController::moveCursorVertically(int direction) {
  Function * actualFunction = functionStore()->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float y = actualFunction->evaluateAtAbscissa(m_cursor.x(), myApp->localContext());
  Function * nextFunction = actualFunction;
  float nextY = direction > 0 ? FLT_MAX : -FLT_MAX;
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    Function * f = functionStore()->activeFunctionAtIndex(i);
    float newY = f->evaluateAtAbscissa(m_cursor.x(), myApp->localContext());
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
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor.x(), m_cursor.y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

CurveView * FunctionGraphController::curveView() {
  return functionGraphView();
}

uint32_t FunctionGraphController::modelVersion() {
  return functionStore()->storeChecksum();
}

uint32_t FunctionGraphController::rangeVersion() {
  return interactiveCurveViewRange()->rangeChecksum();
}

}
