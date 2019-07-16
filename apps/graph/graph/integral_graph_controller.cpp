#include "integral_graph_controller.h"
#include "../../shared/text_field_delegate.h"
#include <poincare/layout_helper.h>
#include "../app.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Shared;
using namespace Poincare;

namespace Graph {

IntegralGraphController::IntegralGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
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

double IntegralGraphController::cursorNextStep(double x, int direction) {
  return (direction > 0 ? x + m_graphRange->xGridUnit()/k_numberOfCursorStepsInGradUnit : x - m_graphRange->xGridUnit()/k_numberOfCursorStepsInGradUnit);
}

Layout IntegralGraphController::createFunctionLayout(ExpiringPointer<Shared::Function> function) {
  constexpr size_t bufferSize = SymbolAbstract::k_maxNameSize+5; // f(x)dx
  char buffer[bufferSize];
  const char * dx = "dx";
  int numberOfChars = function->nameWithArgument(buffer, bufferSize-strlen(dx));
  assert(numberOfChars <= bufferSize);
  strlcpy(buffer+numberOfChars, dx, bufferSize-numberOfChars);
  return LayoutHelper::String(buffer, strlen(buffer), KDFont::SmallFont);
}

}
