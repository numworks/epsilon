#include "term_sum_controller.h"

#include <poincare/layout.h>

#include <cmath>

#include "../app.h"

extern "C" {
#include <assert.h>
#include <stdlib.h>
}

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

TermSumController::TermSumController(Responder* parentResponder,
                                     GraphView* graphView,
                                     CurveViewRange* graphRange,
                                     CurveViewCursor* cursor)
    : SumGraphController(parentResponder, graphView, graphRange, cursor) {}

const char* TermSumController::title() const {
  return I18n::translate(I18n::Message::TermSum);
}

bool TermSumController::moveCursorHorizontallyToPosition(double position) {
  ExpiringPointer<Shared::Sequence> sequence =
      App::app()->functionStore()->modelForRecord(selectedRecord());
  int initialRank = sequence->initialRank();
  int intPosition = std::round(position);
  if (intPosition < initialRank) {
    return false;
  }
  return SumGraphController::moveCursorHorizontallyToPosition(intPosition);
}

I18n::Message TermSumController::legendMessageAtStep(Step step) {
  switch (step) {
    case Step::FirstParameter:
      return I18n::Message::SelectFirstTerm;
    case Step::SecondParameter:
      return I18n::Message::SelectLastTerm;
    default:
      return I18n::Message::Default;
  }
}

double TermSumController::cursorNextStep(double x,
                                         OMG::HorizontalDirection direction) {
  double delta = direction.isRight() ? 1.0 : -1.0;
  return std::round(m_cursor->x() + delta);
}

Layout TermSumController::createFunctionLayout() {
  ExpiringPointer<Shared::Sequence> sequence =
      App::app()->functionStore()->modelForRecord(selectedRecord());
  return sequence->nameLayout();
}

}  // namespace Sequence
