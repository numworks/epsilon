#include "area_between_curves_graph_controller.h"

#include <assert.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <stdlib.h>

#include <cmath>

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

void AreaBetweenCurvesGraphController::makeCursorVisibleOnSecondCurve(float x) {
  assert(!secondSelectedRecord().isNull());
  ExpiringPointer<Shared::Function> functionG =
      FunctionApp::app()->functionStore()->modelForRecord(
          secondSelectedRecord());
  float yG =
      functionG->evaluateXYAtParameter(x, FunctionApp::app()->localContext())
          .y();
  // zoomOut is always true so that the user can see both dots
  makeDotVisible(x, yG, true);
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
  if (std::isfinite(nextIntersection.x())) {
    return nextIntersection.x();
  }
  return nextX;
}

Poincare::Layout AreaBetweenCurvesGraphController::createFunctionLayout() {
  constexpr size_t bufferSize =
      Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small) + 1;
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
  return Layout::Create(KAbsL(KA) ^ "dx"_l, {.KA = Layout::String(buffer)});
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
  Poincare::Expression result =
      Poincare::Expression::Create(KIntegral(KA, KB, KC, KAbs(KSub(KD, KE))),
                                   {.KA = Symbol::SystemSymbol(),
                                    .KB = Expression::Builder<double>(startSum),
                                    .KC = Expression::Builder<double>(endSum),
                                    .KD = expressionF,
                                    .KE = expressionG});
  return result;
}

}  // namespace Graph
