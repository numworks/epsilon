#include "input_goodness_table_cell.h"

#include <shared/column_parameter_controller.h>

#include "input_goodness_controller.h"

using namespace Escher;

namespace Inference {

InputGoodnessTableCell::InputGoodnessTableCell(
    Responder* parentResponder, GoodnessTest* test,
    InputGoodnessController* inputGoodnessController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : DoubleColumnTableCell(parentResponder, test, scrollViewDelegate),
      m_inputGoodnessController(inputGoodnessController) {
  for (int i = 0; i < GoodnessTest::k_numberOfInputColumns; i++) {
    m_header[i].setMessage(k_columnHeaders[i]);
    m_header[i].setEven(true);
  }
}

bool InputGoodnessTableCell::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  int previousDegreeOfFreedom = statistic()->computeDegreesOfFreedom();
  if (InputCategoricalTableCell::textFieldDidFinishEditing(textField, event)) {
    int newDegreeOfFreedom = statistic()->computeDegreesOfFreedom();
    if (previousDegreeOfFreedom != newDegreeOfFreedom) {
      statistic()->setDegreeOfFreedom(newDegreeOfFreedom);
      m_inputGoodnessController->updateDegreeOfFreedomCell();
    }
    return true;
  }
  return false;
}

bool InputGoodnessTableCell::recomputeDimensionsAndReload(
    bool forceReloadTable, bool forceReloadPage, bool forceReloadCell) {
  // Update degree of freedom if Number of non-empty rows changed
  if (InputCategoricalTableCell::recomputeDimensionsAndReload(
          forceReloadTable, forceReloadPage, forceReloadCell)) {
    int newDegreeOfFreedom = statistic()->computeDegreesOfFreedom();
    statistic()->setDegreeOfFreedom(newDegreeOfFreedom);
    m_inputGoodnessController->updateDegreeOfFreedomCell();
    return true;
  }
  return false;
}

size_t InputGoodnessTableCell::fillColumnName(int column, char* buffer) {
  return strlcpy(buffer, I18n::translate(k_columnHeaders[column]),
                 Shared::ColumnParameterController::k_titleBufferSize);
}

CategoricalController* InputGoodnessTableCell::categoricalController() {
  return m_inputGoodnessController;
}

}  // namespace Inference
