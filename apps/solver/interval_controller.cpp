#include "interval_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <string.h>

#include "app.h"

using namespace Escher;

namespace Solver {

IntervalController::IntervalController(Responder *parentResponder)
    : FloatParameterController<double>(parentResponder, &m_instructions),
      m_instructions(I18n::Message::ApproximateSolutionIntervalInstruction,
                     k_messageFormat) {
  m_okButton.setMessage(I18n::Message::ResolveEquation);
  m_intervalCell[0].label()->setMessage(I18n::Message::XMin);
  m_intervalCell[1].label()->setMessage(I18n::Message::XMax);
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_intervalCell[i].setParentResponder(&m_selectableListView);
    m_intervalCell[i].setDelegate(this);
  }
}

const char *IntervalController::title() {
  return I18n::translate(I18n::Message::SearchInverval);
}

int IntervalController::numberOfRows() const { return k_maxNumberOfCells + 1; }

KDCoordinate IntervalController::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  if (type == k_parameterCellType) {
    return m_intervalCell[row].minimalSizeForOptimalDisplay().height();
  }
  assert(type == k_buttonCellType);
  return Shared::FloatParameterController<double>::nonMemoizedRowHeight(row);
}

HighlightCell *IntervalController::reusableParameterCell(int index, int type) {
  assert(0 <= index && index < k_maxNumberOfCells);
  return &m_intervalCell[index];
}

TextField *IntervalController::textFieldOfCellAtIndex(HighlightCell *cell,
                                                      int index) {
  assert(typeAtRow(index) == k_parameterCellType);
  return static_cast<MenuCellWithEditableText<MessageTextView> *>(cell)
      ->textField();
}

int IntervalController::reusableParameterCellCount(int type) {
  return k_maxNumberOfCells;
}

double IntervalController::parameterAtIndex(int index) {
  SystemOfEquations *system = App::app()->system();
  return index == 0 ? system->approximateResolutionMinimum()
                    : system->approximateResolutionMaximum();
}

bool IntervalController::setParameterAtIndex(int parameterIndex, double f) {
  SystemOfEquations *system = App::app()->system();
  parameterIndex == 0 ? system->setApproximateResolutionMinimum(f)
                      : system->setApproximateResolutionMaximum(f);
  return true;
}

bool IntervalController::textFieldDidFinishEditing(AbstractTextField *textField,
                                                   Ion::Events::Event event) {
  if (FloatParameterController::textFieldDidFinishEditing(textField, event)) {
    m_selectableListView.reloadData();
    return true;
  }
  return false;
}

void IntervalController::buttonAction() {
  StackViewController *stack = stackController();
  App::app()->system()->approximateSolve(App::app()->localContext());
  stack->push(App::app()->solutionsController());
}

}  // namespace Solver
