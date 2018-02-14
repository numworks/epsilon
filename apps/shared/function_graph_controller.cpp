#include "function_graph_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>
#include <cmath>
#include <float.h>

using namespace Poincare;

namespace Shared {

FunctionGraphController::FunctionGraphController(Responder * parentResponder, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Expression::AngleUnit * angleUnitVersion) :
  InteractiveCurveViewController(parentResponder, header, interactiveRange, curveView, cursor, modelVersion, rangeVersion),
  m_initialisationParameterController(this, interactiveRange),
  m_angleUnitVersion(angleUnitVersion),
  m_indexFunctionSelectedByCursor(indexFunctionSelectedByCursor)
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
  functionGraphView()->setCursorView(cursorView());
  functionGraphView()->setBannerView(bannerView());
  functionGraphView()->setAreaHighlight(NAN,NAN);

  if (functionGraphView()->context() == nullptr) {
    TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
    functionGraphView()->setContext(myApp->localContext());
  }
  Expression::AngleUnit newAngleUnitVersion = Preferences::sharedPreferences()->angleUnit();
  if (*m_angleUnitVersion != newAngleUnitVersion) {
    *m_angleUnitVersion = newAngleUnitVersion;
    initCursorParameters();
  }
  InteractiveCurveViewController::viewWillAppear();
}

bool FunctionGraphController::handleEnter() {
  Function * f = functionStore()->activeFunctionAtIndex(indexFunctionSelectedByCursor());
  curveParameterController()->setFunction(f);
  StackViewController * stack = stackController();
  stack->push(curveParameterController());
  return true;
}

void FunctionGraphController::selectFunctionWithCursor(int functionIndex) {
  *m_indexFunctionSelectedByCursor = functionIndex;
}

void FunctionGraphController::reloadBannerView() {
  if (functionStore()->numberOfActiveFunctions() == 0) {
    return;
  }
  Function * f = functionStore()->activeFunctionAtIndex(indexFunctionSelectedByCursor());
  reloadBannerViewForCursorOnFunction(m_cursor, f, functionStore()->symbol());
}

InteractiveCurveViewRangeDelegate::Range FunctionGraphController::computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float min = FLT_MAX;
  float max = -FLT_MAX;
  float xMin = interactiveCurveViewRange->xMin();
  float xMax = interactiveCurveViewRange->xMax();
  if (functionStore()->numberOfActiveFunctions() <= 0) {
    InteractiveCurveViewRangeDelegate::Range range;
    range.min = xMin;
    range.max = xMax;
    return range;
  }
  for (int i=0; i<functionStore()->numberOfActiveFunctions(); i++) {
    Function * f = functionStore()->activeFunctionAtIndex(i);
    float y = 0.0f;
    for (int j = 0; j <= curveView()->resolution(); j++) {
      float x = xMin+(xMax-xMin)*j/curveView()->resolution();
      y = f->evaluateAtAbscissa(x, myApp->localContext());
      if (!std::isnan(y) && !std::isinf(y)) {
        min = min < y ? min : y;
        max = max > y ? max : y;
      }
    }
  }
  InteractiveCurveViewRangeDelegate::Range range;
  range.min = min;
  range.max = max;
  return range;
}

float FunctionGraphController::addMargin(float x, float range, bool isMin) {
  float ratio = isMin ? -k_displayBottomMarginRatio : k_displayTopMarginRatio;
  return x+ratio*range;
}

void FunctionGraphController::initRangeParameters() {
  interactiveCurveViewRange()->setDefault();
  initCursorParameters();
  selectFunctionWithCursor(0);
}

bool FunctionGraphController::moveCursorVertically(int direction) {
  Function * actualFunction = functionStore()->activeFunctionAtIndex(indexFunctionSelectedByCursor());
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  double y = actualFunction->evaluateAtAbscissa(m_cursor->x(), myApp->localContext());
  Function * nextFunction = actualFunction;
  double nextY = direction > 0 ? DBL_MAX : -DBL_MAX;
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    Function * f = functionStore()->activeFunctionAtIndex(i);
    double newY = f->evaluateAtAbscissa(m_cursor->x(), myApp->localContext());
    bool isNextFunction = direction > 0 ? (newY > y && newY < nextY) : (newY < y && newY > nextY);
    if (isNextFunction) {
      selectFunctionWithCursor(i);
      nextY = newY;
      nextFunction = f;
    }
  }
  if (nextFunction == actualFunction) {
    return false;
  }
  m_cursor->moveTo(m_cursor->x(), nextY);
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
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

bool FunctionGraphController::isCursorVisible() {
  return interactiveCurveViewRange()->isCursorVisible(k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

}
