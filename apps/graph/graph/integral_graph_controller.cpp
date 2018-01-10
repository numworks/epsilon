#include "integral_graph_controller.h"
#include "../../shared/text_field_delegate.h"
#include "../app.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Shared;
using namespace Poincare;

namespace Graph {

IntegralGraphController::IntegralGraphController(Responder * parentResponder, GraphView * graphView, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  SumGraphController(parentResponder, graphView, graphRange, cursor, Ion::Charset::Integral)
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

}
