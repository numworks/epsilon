#include "area_between_curves_graph_controller.h"

#include <assert.h>
#include <poincare/absolute_value.h>
#include <poincare/absolute_value_layout.h>
#include <poincare/integral.h>
#include <poincare/layout_helper.h>
#include <poincare/subtraction.h>
#include <stdlib.h>

#include <cmath>

#include "../../shared/text_field_delegate.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

const char *AreaBetweenCurvesGraphController::title() {
  return I18n::translate(I18n::Message::AreaBetweenCurves);
}

void AreaBetweenCurvesGraphController::viewWillAppear() {
  IntegralGraphController::viewWillAppear();
  static_cast<GraphView *>(m_graphView)
      ->setInterest(Solver<double>::Interest::Intersection);
}

void AreaBetweenCurvesGraphController::viewDidDisappear() {
  m_graphView->selectSecondRecord(nullptr);
  IntegralGraphController::viewDidDisappear();
}

void AreaBetweenCurvesGraphController::setSecondRecord(
    Ion::Storage::Record record) {
  m_graphView->selectSecondRecord(record);
}

void AreaBetweenCurvesGraphController::makeCursorVisible() {
  float position = m_cursor->x();
  ExpiringPointer<Shared::Function> functionF =
      FunctionApp::app()->functionStore()->modelForRecord(selectedRecord());
  float yF =
      functionF
          ->evaluateXYAtParameter(position, FunctionApp::app()->localContext())
          .x2();
  // Do not zoom out if user is selecting first parameter
  makeDotVisible(position, yF, m_step != Step::FirstParameter);
  assert(!secondSelectedRecord().isNull());
  ExpiringPointer<Shared::Function> functionG =
      FunctionApp::app()->functionStore()->modelForRecord(
          secondSelectedRecord());
  float yG =
      functionG
          ->evaluateXYAtParameter(position, FunctionApp::app()->localContext())
          .x2();
  // zoomOut is always true so that the user can see both dots
  makeDotVisible(position, yG, true);
}

double AreaBetweenCurvesGraphController::cursorNextStep(
    double position, OMG::HorizontalDirection direction) {
  double nextX = IntegralGraphController::cursorNextStep(position, direction);
  constexpr double snapFactor = 1.5;
  double nextSnap = position + snapFactor * (nextX - position);
  Coordinate2D<double> nextIntersection =
      App::app()
          ->graphController()
          ->pointsOfInterestForRecord(selectedRecord())
          ->firstPointInDirection(position, nextSnap,
                                  Solver<double>::Interest::Intersection)
          .xy();
  if (std::isfinite(nextIntersection.x1())) {
    return nextIntersection.x1();
  }
  return nextX;
}

Poincare::Layout AreaBetweenCurvesGraphController::createFunctionLayout() {
  constexpr size_t bufferSize = Escher::BufferTextView::k_maxNumberOfChar;
  char buffer[bufferSize];
  ExpiringPointer<ContinuousFunction> functionF =
      App::app()->functionStore()->modelForRecord(selectedRecord());
  bool functionFisNamed = functionF->isNamed();
  size_t numberOfChars = functionF->nameWithArgument(buffer, bufferSize);
  if (numberOfChars >= bufferSize) {
    return Layout();
  }
  numberOfChars +=
      strlcpy(buffer + numberOfChars, "-", bufferSize - numberOfChars);
  if (numberOfChars >= bufferSize) {
    return Layout();
  }
  ExpiringPointer<ContinuousFunction> functionG =
      App::app()->functionStore()->modelForRecord(secondSelectedRecord());
  if (!functionFisNamed && !functionG->isNamed()) {
    /* If both function are unnamed, display "Area = ..."
     * to avoid displaying "integral(|y - y|) = ..." */
    return Layout();
  }
  numberOfChars += functionG->nameWithArgument(buffer + numberOfChars,
                                               bufferSize - numberOfChars);
  if (numberOfChars >= bufferSize) {
    return Layout();
  }
  Poincare::Layout subtractionLayout =
      LayoutHelper::String(buffer, strlen(buffer));
  Poincare::Layout absoluteValue =
      AbsoluteValueLayout::Builder(subtractionLayout);
  const char *dx = "dx";
  Poincare::Layout dxLayout = LayoutHelper::String(dx, strlen(dx));
  return Poincare::HorizontalLayout::Builder(absoluteValue, dxLayout);
}

Poincare::Expression AreaBetweenCurvesGraphController::createSumExpression(
    double startSum, double endSum, Poincare::Context *context) {
  // Get the expression of the first function
  ExpiringPointer<Shared::Function> function =
      FunctionApp::app()->functionStore()->modelForRecord(selectedRecord());
  Poincare::Expression expressionF =
      function->expressionReduced(context).clone();
  // Get the expression of the second function
  function = FunctionApp::app()->functionStore()->modelForRecord(
      secondSelectedRecord());
  Poincare::Expression expressionG =
      function->expressionReduced(context).clone();
  return Integral::Builder(
      AbsoluteValue::Builder(Subtraction::Builder(expressionF, expressionG)),
      Symbol::Builder(UCodePointUnknown), Float<double>::Builder(startSum),
      Float<double>::Builder(endSum));
}

}  // namespace Graph
