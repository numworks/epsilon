#include "input_goodness_table.h"

#include <shared/column_parameter_controller.h>

#include "inference/controllers/chi_square/input_goodness_controller.h"

using namespace Escher;

namespace Inference {

InputGoodnessTable::InputGoodnessTable(
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

bool InputGoodnessTable::recomputeDimensionsAndReload(bool forceReloadTable,
                                                      bool forceReloadPage,
                                                      bool forceReloadCell) {
  // Update degree of freedom if Number of non-empty rows changed
  if (InputCategoricalTableCell::recomputeDimensionsAndReload(
          forceReloadTable, forceReloadPage, forceReloadCell)) {
    int newDegreeOfFreedom = inference()->computeDegreesOfFreedom();
    inference()->setDegreeOfFreedom(newDegreeOfFreedom);
    m_inputGoodnessController->updateDegreeOfFreedomCell();
    return true;
  }
  return false;
}

size_t InputGoodnessTable::fillColumnName(int column, char* buffer) {
  return strlcpy(buffer, I18n::translate(k_columnHeaders[column]),
                 Shared::ColumnParameterController::k_titleBufferSize);
}

CategoricalController* InputGoodnessTable::categoricalController() {
  return m_inputGoodnessController;
}

}  // namespace Inference
