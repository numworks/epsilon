#include "function_graph_controller.h"
#include "function_app.h"
#include <poincare/coordinate_2D.h>
#include <assert.h>
#include <cmath>
#include <float.h>

using namespace Poincare;

namespace Shared {

static inline int minFloat(float x, float y) { return x < y ? x : y; }
static inline int maxFloat(float x, float y) { return x > y ? x : y; }

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
  assert(functionStore()->numberOfActiveFunctions() > 0);
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  reloadBannerViewForCursorOnFunction(m_cursor, record, functionStore());
}

InteractiveCurveViewRangeDelegate::Range FunctionGraphController::computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  float min = FLT_MAX;
  float max = -FLT_MAX;
  float xMin = interactiveCurveViewRange->xMin();
  float xMax = interactiveCurveViewRange->xMax();
  /* In practice, a step smaller than a pixel's width is needed for sampling
   * the values of a function. Otherwise some relevant extremal values may be
   * missed. */
  const float step = curveView()->pixelWidth() / 2;
  assert(functionStore()->numberOfActiveFunctions() > 0);
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    ExpiringPointer<Function> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    /* Scan x-range from the middle to the extrema in order to get balanced
     * y-range for even functions (y = 1/x). */
    double tMin = f->tMin();
    if (std::isnan(tMin)) {
      tMin = xMin;
    }
    double tMax = f->tMax();
    if (std::isnan(tMax)) {
      tMax = xMax;
    }
    const int balancedBound = std::floor((tMax-tMin)/2/step);
    for (int j = -balancedBound; j <= balancedBound ; j++) {
      float t = (tMin+tMax)/2 + step * j;
      Coordinate2D<float> xy = f->evaluateXYAtParameter(t, context);
      float x = xy.x1();
      if (!std::isnan(x) && !std::isinf(x) && x >= xMin && x <= xMax) {
        float y = xy.x2();
        if (!std::isnan(y) && !std::isinf(y)) {
          min = minFloat(min, y);
          max = maxFloat(max, y);
        }
      }
    }
  }
  InteractiveCurveViewRangeDelegate::Range range;
  range.min = min;
  range.max = max;
  return range;
}

double FunctionGraphController::defaultCursorT() {
  return 0.0; //TODO LEA interactiveCurveViewRange()->xCenter();
}

FunctionStore * FunctionGraphController::functionStore() const {
  return FunctionApp::app()->functionStore();
}

void FunctionGraphController::initCursorParameters() {
  double t = defaultCursorT();
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  int functionIndex = 0;
  Coordinate2D<double> xy;
  do {
    ExpiringPointer<Function> firstFunction = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(functionIndex++));
    xy = firstFunction->evaluateXYAtParameter(t, context);
  } while ((std::isnan(xy.x2()) || std::isinf(xy.x2())) && functionIndex < functionStore()->numberOfActiveFunctions());
  m_cursor->moveTo(t, xy.x1(), xy.x2());
  functionIndex = (std::isnan(xy.x2()) || std::isinf(xy.x2())) ? 0 : functionIndex - 1;
  selectFunctionWithCursor(functionIndex);
  if (interactiveCurveViewRange()->yAuto()) {
    interactiveCurveViewRange()->panToMakePointVisible(xy.x1(), xy.x2(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
  }
}

bool FunctionGraphController::moveCursorVertically(int direction) {
  return false; //TODO LEA
#if 0
  int currentActiveFunctionIndex = indexFunctionSelectedByCursor();
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  int nextActiveFunctionIndex = InteractiveCurveViewController::closestCurveIndexVertically(direction > 0, currentActiveFunctionIndex, context);
  if (nextActiveFunctionIndex < 0) {
    return false;
  }
  selectFunctionWithCursor(nextActiveFunctionIndex);
  m_cursor->moveTo(m_cursor->x(), yValue(nextActiveFunctionIndex, m_cursor->x(), context));
  return true;
#endif
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

Coordinate2D<double> FunctionGraphController::xyValues(int curveIndex, double t, Poincare::Context * context) const {
  return functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(curveIndex))->evaluateXYAtParameter(t, context);
}

int FunctionGraphController::numberOfCurves() const {
  return functionStore()->numberOfActiveFunctions();
}

}
