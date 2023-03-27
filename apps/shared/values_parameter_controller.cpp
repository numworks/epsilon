#include "values_parameter_controller.h"

#include <assert.h>

#include "column_parameter_controller.h"
#include "function_app.h"
#include "values_controller.h"

using namespace Escher;

namespace Shared {

ValuesParameterController::ValuesParameterController(
    Responder* parentResponder, ValuesController* valuesController)
    : ColumnParameterController(parentResponder),
      m_valuesController(valuesController) {
  m_setInterval.label()->setMessage(I18n::Message::IntervalSet);
  m_clearColumn.label()->setMessage(I18n::Message::ClearColumn);
}

bool ValuesParameterController::handleEvent(Ion::Events::Event event) {
  if (selectedRow() == k_indexOfClearColumn &&
      m_clearColumn.canBeActivatedByEvent(event)) {
    stackView()->pop();
    m_valuesController->presentClearSelectedColumnPopupIfClearable();
    return true;
  } else if (selectedRow() == k_indexOfSetInterval &&
             m_setInterval.canBeActivatedByEvent(event)) {
    IntervalParameterController* intervalParameterController =
        m_valuesController->intervalParameterController();
    intervalParameterController->setTitle(I18n::Message::IntervalSet);
    stackView()->push(intervalParameterController);
    return true;
  }
  return false;
}

void ValuesParameterController::initializeColumnParameters() {
  ColumnParameterController::initializeColumnParameters();
  m_valuesController->initializeInterval();
}

ColumnNameHelper* ValuesParameterController::columnNameHelper() {
  return m_valuesController;
}

HighlightCell* ValuesParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  static_assert(
      k_totalNumberOfCell == 2,
      "Shared::ValuesParameterController::reusableCell is deprecated.");
  HighlightCell* cells[] = {&m_clearColumn, &m_setInterval};
  return cells[index];
}

}  // namespace Shared
