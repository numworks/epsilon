#include "goodness_table_cell.h"
#include "inference/statistic/chi_square_and_slope/input_goodness_controller.h"
#include <shared/column_parameter_controller.h>

using namespace Escher;

namespace Inference {

GoodnessTableCell::GoodnessTableCell(Responder * parentResponder, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate, SelectableTableViewDelegate * selectableTableViewDelegate, GoodnessTest * test, InputGoodnessController * inputGoodnessController) :
  DoubleColumnTableCell(parentResponder, dynamicSizeTableViewDataSourceDelegate, selectableTableViewDelegate, test),
  m_inputGoodnessController(inputGoodnessController)
{
  for (int i = 0; i < GoodnessTest::k_maxNumberOfColumns; i++) {
    m_header[i].setMessage(k_columnHeaders[i]);
    m_header[i].setEven(true);
    m_header[i].setMessageFont(KDFont::Size::Small);
  }
}

bool GoodnessTableCell::textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  int previousDegreeOfFreedom = statistic()->computeDegreesOfFreedom();
  if (EditableCategoricalTableCell::textFieldDidFinishEditing(textField, text, event)) {
    int newDegreeOfFreedom = statistic()->computeDegreesOfFreedom();
    if (previousDegreeOfFreedom != newDegreeOfFreedom) {
      statistic()->setDegreeOfFreedom(newDegreeOfFreedom);
      m_inputGoodnessController->updateDegreeOfFreedomCell();
    }
    return true;
  }
  return false;
}

bool GoodnessTableCell::recomputeDimensions() {
  // Update degree of freedom if Number of non-empty rows changed
  if (EditableCategoricalTableCell::recomputeDimensions()) {
    int newDegreeOfFreedom = statistic()->computeDegreesOfFreedom();
    statistic()->setDegreeOfFreedom(newDegreeOfFreedom);
    m_inputGoodnessController->updateDegreeOfFreedomCell();
    return true;
  }
  return false;
}

int GoodnessTableCell::fillColumnName(int column, char * buffer) {
  return strlcpy(buffer, I18n::translate(k_columnHeaders[column]), Shared::ColumnParameterController::k_titleBufferSize);
}

}
