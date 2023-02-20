#include "integral_graph_controller.h"
#include "../../shared/text_field_delegate.h"
#include <poincare/layout_helper.h>
#include "../app.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

IntegralGraphController::IntegralGraphController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  SumGraphController(parentResponder, inputEventHandlerDelegate, graphView, graphRange, cursor, UCodePointIntegral)
{
}

const char * IntegralGraphController::title() {
  return I18n::translate(I18n::Message::Integral);
}

I18n::Message IntegralGraphController::legendMessageAtStep(Step step) {
  switch(step) {
    case Step::FirstParameter:
      return I18n::Message::SelectLowerBound;
    case Step::SecondParameter:
      return I18n::Message::SelectUpperBound;
    default:
      return I18n::Message::Default;
  }
}

double IntegralGraphController::cursorNextStep(double x, OMG::HorizontalDirection direction) {
  return x + (direction.isRight() ? 1.0 : -1.0)*static_cast<double>(m_graphRange->xGridUnit())/static_cast<double>(k_numberOfCursorStepsInGradUnit);
}

Layout IntegralGraphController::createFunctionLayout() {
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(selectedRecord());
  constexpr size_t bufferSize = SymbolAbstract::k_maxNameSize+5; // f(x)dx
  char buffer[bufferSize];
  const char * dx = "dx";
  size_t numberOfChars = function->nameWithArgument(buffer, bufferSize-strlen(dx));
  assert(numberOfChars <= bufferSize);
  strlcpy(buffer+numberOfChars, dx, bufferSize-numberOfChars);
  return LayoutHelper::String(buffer, strlen(buffer));
}

}
