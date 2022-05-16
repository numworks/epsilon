#include "goodness_table_cell.h"
#include "inference/statistic/chi_square_and_slope/input_goodness_controller.h"
#include <shared/column_parameter_controller.h>

using namespace Escher;

namespace Inference {

GoodnessTableCell::GoodnessTableCell(Responder * parentResponder, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate, SelectableTableViewDelegate * selectableTableViewDelegate, GoodnessTest * test, InputGoodnessController * inputGoodnessController) :
  EditableCategoricalTableCell(parentResponder, this, selectableTableViewDelegate, dynamicSizeTableViewDataSourceDelegate, test),
  DynamicCellsDataSource<Escher::EvenOddEditableTextCell, k_inputGoodnessTableNumberOfReusableCells>(this),
  m_inputGoodnessController(inputGoodnessController)
{
  for (int i = 0; i < GoodnessTest::k_maxNumberOfColumns; i++) {
    m_header[i].setMessage(k_columnHeaders[i]);
    m_header[i].setEven(true);
    m_header[i].setMessageFont(KDFont::SmallFont);
  }
}

int GoodnessTableCell::reusableCellCount(int type) {
    if (type == k_typeOfHeaderCells) {
    return GoodnessTest::k_maxNumberOfColumns;
  }
  return k_maxNumberOfReusableRows * GoodnessTest::k_maxNumberOfColumns;
}

HighlightCell * GoodnessTableCell::reusableCell(int i, int type) {
  assert(i < reusableCellCount(type));
  if (type == k_typeOfHeaderCells) {
    assert(i < 2);
    return &m_header[i];
  }
  return cell(i);
}

bool GoodnessTableCell::textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) {
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

void GoodnessTableCell::willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) {
  if (j == 0) {  // Header
    return;
  }
  Escher::EvenOddEditableTextCell * myCell = static_cast<Escher::EvenOddEditableTextCell *>(cell);
  willDisplayValueCellAtLocation(myCell->editableTextCell()->textField(), myCell, i, j - 1, m_tableModel);
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

}  // namespace Inference
