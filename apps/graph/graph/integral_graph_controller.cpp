#include "integral_graph_controller.h"
#include "../../shared/text_field_delegate.h"
#include <poincare/layout_engine.h>
#include "../app.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Shared;
using namespace Poincare;

namespace Graph {

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

ExpressionLayout * IntegralGraphController::createFunctionLayout(const char * functionName) {
  char buffer[7] = "0(x)dx";
  buffer[0] = functionName[0];
  return LayoutEngine::createStringLayout(buffer, strlen(buffer), KDText::FontSize::Small);
}

}
