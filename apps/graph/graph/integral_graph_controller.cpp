#include "integral_graph_controller.h"

#include <assert.h>
#include <poincare/helpers/symbol.h>
#include <poincare/layout.h>
#include <stdlib.h>

#include <cmath>

#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

IntegralGraphController::IntegralGraphController(
    Responder* parentResponder, GraphView* graphView,
    InteractiveCurveViewRange* graphRange, CurveViewCursor* cursor)
    : SumGraphController(parentResponder, graphView, graphRange, cursor) {}

const char* IntegralGraphController::title() const {
  return I18n::translate(I18n::Message::Integral);
}

I18n::Message IntegralGraphController::legendMessageAtStep(Step step) {
  switch (step) {
    case Step::FirstParameter:
      return I18n::Message::SelectLowerBound;
    case Step::SecondParameter:
      return I18n::Message::SelectUpperBound;
    default:
      return I18n::Message::Default;
  }
}

double IntegralGraphController::cursorNextStep(
    double x, OMG::HorizontalDirection direction) {
  return x + (direction.isRight() ? 1.0 : -1.0) *
                 static_cast<double>(m_graphRange->xGridUnit()) /
                 static_cast<double>(k_numberOfCursorStepsInGradUnit);
}

Layout IntegralGraphController::createFunctionLayout() {
  ExpiringPointer<ContinuousFunction> function =
      App::app()->functionStore()->modelForRecord(selectedRecord());
  constexpr size_t bufferSize =
      Poincare::SymbolHelper::k_maxNameSize + 5;  // f(x)dx
  char buffer[bufferSize];
  const char* dx = "dx";
  size_t numberOfChars =
      function->nameWithArgument(buffer, bufferSize - strlen(dx));
  assert(numberOfChars <= bufferSize);
  strlcpy(buffer + numberOfChars, dx, bufferSize - numberOfChars);
  return Layout::String(buffer, strlen(buffer));
}

}  // namespace Graph
