#include "term_sum_controller.h"
#include "../../shared/text_field_delegate.h"
#include "../app.h"
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

extern "C" {
#include <assert.h>
#include <stdlib.h>
}

using namespace Shared;
using namespace Poincare;

namespace Sequence {

TermSumController::TermSumController(Responder * parentResponder, ::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, CurveViewRange * graphRange, CurveViewCursor * cursor) :
  SumGraphController(parentResponder, inputEventHandlerDelegate, graphView, graphRange, cursor, Ion::Charset::CapitalSigma)
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

Layout TermSumController::createFunctionLayout(const char * functionName) {
  return HorizontalLayout::Builder(
      CharLayout(functionName[0], KDFont::SmallFont),
        VerticalOffsetLayout::Builder(
          CharLayout('n', KDFont::SmallFont),
          VerticalOffsetLayoutNode::Type::Subscript
        )
      );
}

}
