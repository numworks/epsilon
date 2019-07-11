#include "function_graph_controller.h"
#include "function_app.h"
#include <assert.h>
#include <cmath>
#include <float.h>

using namespace Poincare;

namespace Shared {

FunctionGraphController::FunctionGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Preferences::AngleUnit * angleUnitVersion) :
  InteractiveCurveViewController(parentResponder, inputEventHandlerDelegate, header, interactiveRange, curveView, cursor, modelVersion, rangeVersion),
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

void FunctionGraphController::didBecomeFirstResponder() {
  if (curveView()->isMainViewSelected()) {
    bannerView()->abscissaValue()->setParentResponder(this);
    bannerView()->abscissaValue()->setDelegates(textFieldDelegateApp(), this);
    Container::activeApp()->setFirstResponder(bannerView()->abscissaValue());
  } else {
    InteractiveCurveViewController::didBecomeFirstResponder();
  }
}

void FunctionGraphController::viewWillAppear() {
  functionGraphView()->setBannerView(bannerView());
  functionGraphView()->setAreaHighlight(NAN,NAN);

  if (functionGraphView()->context() == nullptr) {
    functionGraphView()->setContext(textFieldDelegateApp()->localContext());
  }
  Preferences::AngleUnit newAngleUnitVersion = Preferences::sharedPreferences()->angleUnit();
  if (*m_angleUnitVersion != newAngleUnitVersion) {
    *m_angleUnitVersion = newAngleUnitVersion;
    initCursorParameters();
  }
  InteractiveCurveViewController::viewWillAppear();
}

bool FunctionGraphController::handleEnter() {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  curveParameterController()->setRecord(record);
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
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  reloadBannerViewForCursorOnFunction(m_cursor, record, functionStore(), functionStore()->symbol());
}

InteractiveCurveViewRangeDelegate::Range FunctionGraphController::computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  Poincare::Context * context = textFieldDelegateApp()->localContext();
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
  float step = (xMax - xMin) / curveView()->resolution();
  for (int i=0; i<functionStore()->numberOfActiveFunctions(); i++) {
    ExpiringPointer<Function> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    /* Scan x-range from the middle to the extrema in order to get balanced
     * y-range for even functions (y = 1/x). */
    const int balancedBound = std::floor((xMax-xMin)/2/step);
    for (int j = -balancedBound; j <= balancedBound ; j++) {
      float x = (xMin+xMax)/2 + step * j;
      float y = f->evaluateAtAbscissa(x, context);
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

double FunctionGraphController::defaultCursorAbscissa() {
  return (interactiveCurveViewRange()->xMin()+interactiveCurveViewRange()->xMax())/2.0f;
}

FunctionStore * FunctionGraphController::functionStore() const {
  return FunctionApp::app()->functionStore();
}

void FunctionGraphController::initCursorParameters() {
  double x = defaultCursorAbscissa();
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  int functionIndex = 0;
  double y = 0;
  do {
    ExpiringPointer<Function> firstFunction = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(functionIndex++));
    y = firstFunction->evaluateAtAbscissa(x, context);
  } while ((std::isnan(y) || std::isinf(y)) && functionIndex < functionStore()->numberOfActiveFunctions());
  m_cursor->moveTo(x, y);
  functionIndex = (std::isnan(y) || std::isinf(y)) ? 0 : functionIndex - 1;
  selectFunctionWithCursor(functionIndex);
  if (interactiveCurveViewRange()->yAuto()) {
    interactiveCurveViewRange()->panToMakePointVisible(x, y, cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
  }
}

bool FunctionGraphController::moveCursorVertically(int direction) {
  int currentActiveFunctionIndex = indexFunctionSelectedByCursor();
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  int nextActiveFunctionIndex = InteractiveCurveViewController::closestCurveIndexVertically(direction > 0, currentActiveFunctionIndex, context);
  if (nextActiveFunctionIndex < 0) {
    return false;
  }
  selectFunctionWithCursor(nextActiveFunctionIndex);
  m_cursor->moveTo(m_cursor->x(), yValue(nextActiveFunctionIndex, m_cursor->x(), context));
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

bool FunctionGraphController::closestCurveIndexIsSuitable(int newIndex, int currentIndex) const {
  return newIndex != currentIndex;
}

double FunctionGraphController::yValue(int curveIndex, double x, Poincare::Context * context) const {
  return functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(curveIndex))->evaluateAtAbscissa(x, context);
}

int FunctionGraphController::numberOfCurves() const {
  return functionStore()->numberOfActiveFunctions();
}

}
