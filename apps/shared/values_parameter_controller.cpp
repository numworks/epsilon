#include "column_parameter_controller.h"
#include "values_parameter_controller.h"
#include "values_controller.h"
#include "function_app.h"
#include <assert.h>

using namespace Escher;

namespace Shared {

ValuesParameterController::ValuesParameterController(Responder * parentResponder, ValuesController * valuesController) :
  ColumnParameterController(parentResponder),
  m_clearColumn(I18n::Message::ClearColumn),
  m_setInterval(I18n::Message::IntervalSet),
  m_valuesController(valuesController)
{ }

bool ValuesParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 1)) {
    switch (selectedRow()) {
      case k_indexOfClearColumn:
      {
        stackView()->pop();
        m_valuesController->presentClearSelectedColumnPopupIfClearable();
        return true;
      }
      case k_indexOfSetInterval:
      {
        IntervalParameterController * intervalParameterController = m_valuesController->intervalParameterController();
        intervalParameterController->setTitle(I18n::Message::IntervalSet);
        stackView()->push(intervalParameterController);
        return true;
      }
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

void ValuesParameterController::initializeColumnParameters() {
  ColumnParameterController::initializeColumnParameters();
  m_valuesController->initializeInterval();
}


ClearColumnHelper * ValuesParameterController::clearColumnHelper() {
  return m_valuesController;
}

HighlightCell * ValuesParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  static_assert(k_totalNumberOfCell == 2, "Shared::ValuesParameterController::reusableCell is deprecated.");
  HighlightCell * cells[] = {&m_clearColumn, &m_setInterval};
  return cells[index];
}

}
