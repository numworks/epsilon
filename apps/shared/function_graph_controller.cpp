#include "function_graph_controller.h"
#include "function_app.h"
#include "../apps_container.h"
#include <poincare/coordinate_2D.h>
#include <assert.h>
#include <cmath>
#include <float.h>
#include <algorithm>

using namespace Poincare;

namespace Shared {

FunctionGraphController::FunctionGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * previousModelsVersions, uint32_t * rangeVersion, Preferences::AngleUnit * angleUnitVersion) :
  InteractiveCurveViewController(parentResponder, inputEventHandlerDelegate, header, interactiveRange, curveView, cursor, modelVersion, previousModelsVersions, rangeVersion),
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
  reloadBannerViewForCursorOnFunction(m_cursor, record, functionStore(), AppsContainer::sharedAppsContainer()->globalContext());
}


double FunctionGraphController::defaultCursorT(Ion::Storage::Record record) {
  return (interactiveCurveViewRange()->xMin()+interactiveCurveViewRange()->xMax())/2.0f;
}

FunctionStore * FunctionGraphController::functionStore() const {
  return FunctionApp::app()->functionStore();
}

void FunctionGraphController::initCursorParameters() {
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  const int activeFunctionsCount = functionStore()->numberOfActiveFunctions();
  int functionIndex = 0;
  Coordinate2D<double> xy;
  double t;
  do {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(functionIndex);
    ExpiringPointer<Function> firstFunction = functionStore()->modelForRecord(record);
    t = defaultCursorT(record);
    xy = firstFunction->evaluateXYAtParameter(t, context);
  } while ((std::isnan(xy.x2()) || std::isinf(xy.x2())) && ++functionIndex < activeFunctionsCount);
  if (functionIndex == activeFunctionsCount) {
    functionIndex = 0;
  }
  m_cursor->moveTo(t, xy.x1(), xy.x2());
  if (interactiveCurveViewRange()->yAuto()) {
    interactiveCurveViewRange()->panToMakePointVisible(xy.x1(), xy.x2(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
  }
  selectFunctionWithCursor(functionIndex);
}

bool FunctionGraphController::moveCursorVertically(int direction) {
  int currentActiveFunctionIndex = indexFunctionSelectedByCursor();
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  int nextActiveFunctionIndex = nextCurveIndexVertically(direction > 0, currentActiveFunctionIndex, context);
  if (nextActiveFunctionIndex < 0) {
    return false;
  }
  // Clip the current t to the domain of the next function
  ExpiringPointer<Function> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(nextActiveFunctionIndex));
  double clippedT = m_cursor->t();
  if (!std::isnan(f->tMin())) {
    assert(!std::isnan(f->tMax()));
    clippedT = std::min<double>(f->tMax(), std::max<double>(f->tMin(), clippedT));
  }
  Poincare::Coordinate2D<double> cursorPosition = f->evaluateXYAtParameter(clippedT, context);
  m_cursor->moveTo(clippedT, cursorPosition.x1(), cursorPosition.x2());
  selectFunctionWithCursor(nextActiveFunctionIndex);
  return true;
}

CurveView * FunctionGraphController::curveView() {
  return functionGraphView();
}

uint32_t FunctionGraphController::modelVersion() {
  return functionStore()->storeChecksum();
}

uint32_t FunctionGraphController::modelVersionAtIndex(int i) {
  return functionStore()->storeChecksumAtIndex(i);
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
