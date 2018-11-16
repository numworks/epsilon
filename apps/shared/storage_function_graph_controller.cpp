#include "storage_function_graph_controller.h"
#include "storage_function_app.h"
#include <assert.h>
#include <cmath>
#include <float.h>

using namespace Poincare;

namespace Shared {

StorageFunctionGraphController::StorageFunctionGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Preferences::AngleUnit * angleUnitVersion) :
  InteractiveCurveViewController(parentResponder, inputEventHandlerDelegate, header, interactiveRange, curveView, cursor, modelVersion, rangeVersion),
  m_initialisationParameterController(this, interactiveRange),
  m_angleUnitVersion(angleUnitVersion),
  m_indexFunctionSelectedByCursor(indexFunctionSelectedByCursor)
{
}

bool StorageFunctionGraphController::isEmpty() const {
  if (functionStore()->numberOfActiveFunctions() == 0) {
    return true;
  }
  return false;
}

ViewController * StorageFunctionGraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
}

void StorageFunctionGraphController::viewWillAppear() {
  functionGraphView()->setCursorView(cursorView());
  functionGraphView()->setBannerView(bannerView());
  functionGraphView()->setAreaHighlight(NAN,NAN);

  if (functionGraphView()->context() == nullptr) {
    StorageFunctionApp * myApp = static_cast<StorageFunctionApp *>(app());
    functionGraphView()->setContext(myApp->localContext());
  }
  Preferences::AngleUnit newAngleUnitVersion = Preferences::sharedPreferences()->angleUnit();
  if (*m_angleUnitVersion != newAngleUnitVersion) {
    *m_angleUnitVersion = newAngleUnitVersion;
    initCursorParameters();
  }
  InteractiveCurveViewController::viewWillAppear();
}

bool StorageFunctionGraphController::handleEnter() {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  curveParameterController()->setRecord(record);
  StackViewController * stack = stackController();
  stack->push(curveParameterController());
  return true;
}

void StorageFunctionGraphController::selectFunctionWithCursor(int functionIndex) {
  *m_indexFunctionSelectedByCursor = functionIndex;
}

float StorageFunctionGraphController::cursorBottomMarginRatio() {
  return (cursorView()->minimalSizeForOptimalDisplay().height()/2+estimatedBannerHeight())/k_viewHeight;
}

void StorageFunctionGraphController::reloadBannerView() {
  if (functionStore()->numberOfActiveFunctions() == 0) {
    return;
  }
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  reloadBannerViewForCursorOnFunction(m_cursor, record, functionStore(), functionStore()->symbol());
}

float StorageFunctionGraphController::displayBottomMarginRatio() {
  return (cursorView()->minimalSizeForOptimalDisplay().height() + 2 + estimatedBannerHeight()) / k_viewHeight;
}

float StorageFunctionGraphController::estimatedBannerHeight() const {
  return BannerView::HeightGivenNumberOfLines(estimatedBannerNumberOfLines());
}

InteractiveCurveViewRangeDelegate::Range StorageFunctionGraphController::computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  StorageFunctionApp * myApp = static_cast<StorageFunctionApp *>(app());
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
    ExpiringPointer<StorageFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    float y = 0.0f;
    float res = curveView()->resolution();
    /* Scan x-range from the middle to the extrema in order to get balanced
     * y-range for even functions (y = 1/x). */
    for (int j = -res/2; j <= res/2; j++) {
      float x = (xMin+xMax)/2.0+(xMax-xMin)*j/res;
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

void StorageFunctionGraphController::initRangeParameters() {
  interactiveCurveViewRange()->setDefault();
  initCursorParameters();
  selectFunctionWithCursor(0);
}

double StorageFunctionGraphController::defaultCursorAbscissa() {
  return (interactiveCurveViewRange()->xMin()+interactiveCurveViewRange()->xMax())/2.0f;
}

StorageFunctionStore * StorageFunctionGraphController::functionStore() const {
  StorageFunctionApp * myApp = static_cast<StorageFunctionApp *>(app());
  return myApp->functionStore();
}

void StorageFunctionGraphController::initCursorParameters() {
  double x = defaultCursorAbscissa();
  StorageFunctionApp * myApp = static_cast<StorageFunctionApp *>(app());
  int functionIndex = 0;
  double y = 0;
  do {
    ExpiringPointer<StorageFunction> firstFunction = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(functionIndex++));
    y = firstFunction->evaluateAtAbscissa(x, myApp->localContext());
  } while ((std::isnan(y) || std::isinf(y)) && functionIndex < functionStore()->numberOfActiveFunctions());
  m_cursor->moveTo(x, y);
  functionIndex = (std::isnan(y) || std::isinf(y)) ? 0 : functionIndex - 1;
  selectFunctionWithCursor(functionIndex);
  interactiveCurveViewRange()->panToMakePointVisible(x, y, displayTopMarginRatio(), k_cursorRightMarginRatio, displayBottomMarginRatio(), k_cursorLeftMarginRatio);
}

bool StorageFunctionGraphController::moveCursorVertically(int direction) {
  int currentActiveFunctionIndex = indexFunctionSelectedByCursor();
  Poincare::Context * context = static_cast<StorageFunctionApp *>(app())->localContext();

  int nextActiveFunctionIndex = InteractiveCurveViewController::closestCurveIndexVertically(direction > 0, currentActiveFunctionIndex, context);
  if (nextActiveFunctionIndex < 0) {
    return false;
  }
  selectFunctionWithCursor(nextActiveFunctionIndex);
  m_cursor->moveTo(m_cursor->x(), yValue(nextActiveFunctionIndex, m_cursor->x(), context));
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
  return true;
}

CurveView * StorageFunctionGraphController::curveView() {
  return functionGraphView();
}

uint32_t StorageFunctionGraphController::modelVersion() {
  return functionStore()->storeChecksum();
}

uint32_t StorageFunctionGraphController::rangeVersion() {
  return interactiveCurveViewRange()->rangeChecksum();
}

bool StorageFunctionGraphController::isCursorVisible() {
  return interactiveCurveViewRange()->isCursorVisible(cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
}

bool StorageFunctionGraphController::closestCurveIndexIsSuitable(int newIndex, int currentIndex) const {
  return newIndex != currentIndex;
}

double StorageFunctionGraphController::yValue(int curveIndex, double x, Poincare::Context * context) const {
  return functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(curveIndex))->evaluateAtAbscissa(x, context);
}

int StorageFunctionGraphController::numberOfCurves() const {
  return functionStore()->numberOfActiveFunctions();
}

}
