#include "term_sum_controller.h"
#include "../../shared/text_field_delegate.h"
#include "../../../poincare/src/layout/baseline_relative_layout.h"
#include "../../../poincare/src/layout/string_layout.h"
#include "../app.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

TermSumController::TermSumController(Responder * parentResponder, GraphView * graphView, CurveViewRange * graphRange, CurveViewCursor * cursor) :
  SumGraphController(parentResponder, graphView, graphRange, cursor, Ion::Charset::CapitalSigma)
{
}

const char * TermSumController::title() {
  return I18n::translate(I18n::Message::TermSum);
}

bool TermSumController::moveCursorHorizontallyToPosition(double position) {
  if (position < 0.0) {
    return false;
  }
  return SumGraphController::moveCursorHorizontallyToPosition(std::round(position));
}

I18n::Message TermSumController::legendMessageAtStep(Step step) {
  switch(step) {
    case Step::FirstParameter:
      return I18n::Message::SelectFirstTerm;
    case Step::SecondParameter:
      return I18n::Message::SelectLastTerm;
    default:
      return I18n::Message::Default;
  }
}

double TermSumController::cursorNextStep(double x, int direction) {
  double delta = direction > 0 ? 1.0 : -1.0;
  return std::round(m_cursor->x()+delta);
}

ExpressionLayout * TermSumController::createFunctionLayout(const char * functionName) {
  return new BaselineRelativeLayout(new StringLayout(functionName, 1, KDText::FontSize::Small), new StringLayout("n", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
}

}
