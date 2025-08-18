#include "interval_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/range.h>

#include "app.h"

using namespace Escher;
using namespace Shared;

namespace Solver {

IntervalController::IntervalController(Responder* parentResponder)
    : SingleRangeController(parentResponder, &m_confirmPopUpController),
      m_confirmPopUpController(Invocation::Builder<IntervalController>(
          [](IntervalController* controller, void* sender) {
            controller->stackController()->pop();
            return true;
          },
          this)) {
  m_okButton.setMessage(I18n::Message::ResolveEquation);
}

const char* IntervalController::title() const {
  return I18n::translate(I18n::Message::SearchInverval);
}

bool IntervalController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left &&
      !(typeAtRow(selectedRow()) == k_autoCellType &&
        m_autoCell.canBeActivatedByEvent(event))) {
    // Bypass the Left == Back of single range controller
    return false;
  }
  bool result = SingleRangeController::handleEvent(event);
  if (result && isAutoRangeInvalid() &&
      typeAtRow(selectedRow()) == k_autoCellType) {
    // Refresh the data to show/hide the cells if the auto range is invalid
    m_selectableListView.reloadData();
  }
  return result;
}

int IntervalController::numberOfRows() const {
  int parentRows = SingleRangeControllerDoublePrecision::numberOfRows();
  if (m_autoParam && isAutoRangeInvalid()) {
    // Hide the interval cells if the auto interval is not yet computed
    return parentRows - 2;
  }
  return parentRows;
}

I18n::Message IntervalController::parameterMessage(int index) const {
  assert(index == 0 || index == 1);
  return index == 0 ? I18n::Message::XMin : I18n::Message::XMax;
}

double IntervalController::limit() const {
  return Poincare::Range1D<double>::k_maxFloat;
}

void IntervalController::extractParameters() {
  SystemOfEquations* system = App::app()->system();
  m_rangeParam = system->approximateSolvingRange();
  if (isAutoRangeInvalid()) {
    // Unset the auto parameter if the auto range is invalid
    system->useAutoSolvingRange(false);
  }
  m_autoParam = system->isUsingAutoSolvingRange();
}

void IntervalController::confirmParameters() {
  SystemOfEquations* system = App::app()->system();
  if (m_autoParam) {
    system->useAutoSolvingRange();
  } else {
    system->setApproximateSolvingRange(m_rangeParam);
  }
}

bool IntervalController::parametersAreDifferent() {
  SystemOfEquations* system = App::app()->system();
  return m_rangeParam != system->approximateSolvingRange();
}

void IntervalController::setAutoRange() {
  m_autoParam = true;
  if (!isAutoRangeInvalid()) {
    m_rangeParam = App::app()->system()->memoizedAutoSolvingRange();
  }
}

void IntervalController::pop(bool onConfirmation) {
  if (onConfirmation) {
    SystemOfEquations* system = App::app()->system();
    Poincare::CircuitBreakerCheckpoint checkpoint(
        Ion::CircuitBreaker::CheckpointType::Back);
    if (CircuitBreakerRun(checkpoint)) {
      system->approximateSolve(App::app()->localContext());
    } else {
      App::app()->equationStore()->tidyDownstreamPoolFrom(
          checkpoint.endOfPoolBeforeCheckpoint());
      system->cancelApproximateSolve();
    }
  }
  StackViewController* stack = stackController();
  stack->pop();
}

bool IntervalController::isAutoRangeInvalid() const {
  return App::app()->system()->memoizedAutoSolvingRange().isNan();
}

}  // namespace Solver
