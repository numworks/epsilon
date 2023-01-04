#include "function_graph_controller.h"
#include "function_app.h"
#include "poincare_helpers.h"
#include <apps/apps_container_helper.h>
#include <poincare/coordinate_2D.h>
#include <poincare/layout_helper.h>
#include <poincare/helpers.h>
#include <assert.h>
#include <cmath>
#include <float.h>
#include <algorithm>

using namespace Escher;
using namespace Poincare;

namespace Shared {

FunctionGraphController::FunctionGraphController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, AbstractPlotView * curveView, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor) :
  InteractiveCurveViewController(parentResponder, inputEventHandlerDelegate, header, interactiveRange, curveView, cursor),
  m_calculusButton(this, I18n::Message::GraphCalculus, calculusButtonInvocation(), k_font),
  m_indexFunctionSelectedByCursor(indexFunctionSelectedByCursor)
{
}

bool FunctionGraphController::isEmpty() const {
  if (numberOfCurves() == 0) {
    return true;
  }
  return false;
}

void FunctionGraphController::didBecomeFirstResponder() {
  if (curveView()->hasFocus()) {
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

  InteractiveCurveViewController::viewWillAppear();
}

bool FunctionGraphController::openMenuForCurveAtIndex(int index) {
  if (index != *m_indexFunctionSelectedByCursor) {
    selectFunctionWithCursor(index, false);
    Coordinate2D<double> xy = xyValues(index, m_cursor->t(), textFieldDelegateApp()->localContext(), m_selectedSubCurveIndex);
    m_cursor->moveTo(m_cursor->t(), xy.x1(), xy.x2());
  }
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  curveParameterControllerWithRecord()->setRecord(record);
  StackViewController * stack = stackController();
  stack->push(curveParameterController());
  return true;
}

void FunctionGraphController::selectFunctionWithCursor(int functionIndex, bool willBeVisible) {
  if (functionIndex != *m_indexFunctionSelectedByCursor) {
    m_selectedSubCurveIndex = 0;
    *m_indexFunctionSelectedByCursor = functionIndex;
  }

  Ion::Storage::Record r = functionStore()->activeRecordAtIndex(functionIndex);
  functionGraphView()->selectRecord(r);
  functionGraphView()->cursorView()->setColor(functionStore()->colorForRecord(r));
  // Force reload to display the selected function on top
  if (willBeVisible) {
    functionGraphView()->reload(false, true);
  }

}

KDCoordinate FunctionGraphController::FunctionSelectionController::nonMemoizedRowHeight(int j) {
  assert(j < graphController()->numberOfCurves());
  ExpiringPointer<Function> function = graphController()->functionStore()->modelForRecord(graphController()->functionStore()->activeRecordAtIndex(j));
  return std::max(function->layout().layoutSize(k_font).height(), nameLayoutAtIndex(j).layoutSize(k_font).height()) + Metric::CellTopMargin + Metric::CellBottomMargin + Metric::CellSeparatorThickness;
}

void FunctionGraphController::FunctionSelectionController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(index < graphController()->numberOfCurves());
  ExpiringPointer<Function> function = graphController()->functionStore()->modelForRecord(graphController()->functionStore()->activeRecordAtIndex(index));
  static_cast<CurveSelectionCellWithChevron *>(cell)->setColor(function->color());
  static_cast<CurveSelectionCellWithChevron *>(cell)->setLayout(function->layout().clone());
}

void FunctionGraphController::reloadBannerView() {
  assert(numberOfCurves() > 0);
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  reloadBannerViewForCursorOnFunction(m_cursor, record, functionStore(), AppsContainerHelper::sharedAppsContainerGlobalContext());
}

double FunctionGraphController::defaultCursorT(Ion::Storage::Record record, bool ignoreMargins) {
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  ExpiringPointer<Function> function = functionStore()->modelForRecord(record);
  float gridUnit = 2.0 * interactiveCurveViewRange()->xGridUnit();
  float xMin = interactiveCurveViewRange()->xMin();
  float xMax = interactiveCurveViewRange()->xMax();

  float middle = (interactiveCurveViewRange()->xMin() + interactiveCurveViewRange()->xMax()) / 2.0f;
  middle = gridUnit * std::floor(middle / gridUnit);
  float currentX, currentY;
  int iterations = 0;
  do {
    /* Start from the middle and place the cursor on each grid unit until you
     * find an x where the cursor is visible.
     * currentX values will alternate over and under the middle value.
     * Example: If the middle is 20 and grid unit is 5, cursorX values
     * will be 20/15/25/10/30/05/35/00/40 etc. */
    currentX = middle + (iterations % 2 == 0 ? 1 : -1) * ((iterations + 1)/ 2) * gridUnit;
    // Using first subCurve for default cursor.
    currentY = function->evaluateXYAtParameter(currentX, context, 0).x2();
    iterations++;
  } while (xMin < currentX && currentX < xMax &&
           !isCursorVisibleAtPosition(Coordinate2D<float>(currentX, currentY), ignoreMargins));

  if (!isCursorVisibleAtPosition(Coordinate2D<float>(currentX, currentY), ignoreMargins)) {
    // If no positions make the cursor visible, return the middle value
    currentX = middle;
  }
  return currentX;
}

FunctionStore * FunctionGraphController::functionStore() const {
  return FunctionApp::app()->functionStore();
}

void FunctionGraphController::computeDefaultPositionForFunctionAtIndex(int index, double * t, Coordinate2D<double> * xy, bool ignoreMargins) {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(index);
  ExpiringPointer<Function> function = functionStore()->modelForRecord(record);
  *t = defaultCursorT(record, ignoreMargins);
  *xy = function->evaluateXYAtParameter(*t, textFieldDelegateApp()->localContext(), 0);
}

void FunctionGraphController::initCursorParameters(bool ignoreMargins) {
  const int activeFunctionsCount = numberOfCurves();
  assert(activeFunctionsCount > 0);
  int functionIndex = 0;
  Coordinate2D<double> xy;
  double t;

  do {
    computeDefaultPositionForFunctionAtIndex(functionIndex, &t, &xy, ignoreMargins);
  } while ((std::isnan(xy.x2()) || std::isinf(xy.x2()) || !isCursorVisibleAtPosition(xy, ignoreMargins)) && ++functionIndex < activeFunctionsCount);

  if (functionIndex == activeFunctionsCount) {
    functionIndex = 0;
    computeDefaultPositionForFunctionAtIndex(functionIndex, &t, &xy, ignoreMargins);
  }

  m_cursor->moveTo(t, xy.x1(), xy.x2());
  selectFunctionWithCursor(functionIndex, false);
}

bool FunctionGraphController::moveCursorVertically(int direction) {
  int currentActiveFunctionIndex = indexFunctionSelectedByCursor();
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  int nextSubCurve = 0;
  int nextActiveFunctionIndex = nextCurveIndexVertically(direction > 0, currentActiveFunctionIndex, context, m_selectedSubCurveIndex, &nextSubCurve);
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
  Poincare::Coordinate2D<double> cursorPosition = f->evaluateXYAtParameter(clippedT, context, nextSubCurve);
  m_cursor->moveTo(clippedT, cursorPosition.x1(), cursorPosition.x2());
  selectFunctionWithCursor(nextActiveFunctionIndex, true);
  m_selectedSubCurveIndex = nextSubCurve;
  return true;
}

bool FunctionGraphController::selectedModelIsValid() const {
  int curveIndex = indexFunctionSelectedByCursor();
  return curveIndex < numberOfCurves() && m_selectedSubCurveIndex < numberOfSubCurves(curveIndex);
}

Poincare::Coordinate2D<double> FunctionGraphController::selectedModelXyValues(double t) const {
  assert(selectedModelIsValid());
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  return xyValues(indexFunctionSelectedByCursor(), t, context, m_selectedSubCurveIndex);
}

AbstractPlotView * FunctionGraphController::curveView() {
  return functionGraphView();
}

Coordinate2D<double> FunctionGraphController::xyValues(int curveIndex, double t, Poincare::Context * context, int subCurveIndex) const {
  return functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(curveIndex))->evaluateXYAtParameter(t, context, subCurveIndex);
}

int FunctionGraphController::numberOfSubCurves(int curveIndex) const {
  return functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(curveIndex))->numberOfSubCurves();
}

bool FunctionGraphController::isAlongY(int curveIndex) const {
  return functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(curveIndex))->isAlongY();
}

int FunctionGraphController::numberOfCurves() const {
  return functionStore()->numberOfActiveFunctions();
}

void FunctionGraphController::tidyModels() {
  int nbOfFunctions = numberOfCurves();
  for (int i = 0; i < nbOfFunctions; i++) {
    ExpiringPointer<Function> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    f->tidyDownstreamPoolFrom();
  }
}

}
