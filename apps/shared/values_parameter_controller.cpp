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
  if (selectedCell() == &m_clearColumn &&
      m_clearColumn.canBeActivatedByEvent(event)) {
    stackView()->pop();
    m_valuesController->presentClearSelectedColumnPopupIfClearable();
    return true;
  } else if (selectedCell() == &m_setInterval &&
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

HighlightCell* ValuesParameterController::cell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell* cells[] = {&m_clearColumn, &m_setInterval};
  static_assert(
      std::size(cells) == k_totalNumberOfCell,
      "Shared::ValuesParameterController::k_totalNumberOfCell is deprecated.");
  return cells[index];
}

}  // namespace Shared
