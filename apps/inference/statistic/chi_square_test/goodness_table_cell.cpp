#include "goodness_table_cell.h"
#include "inference/statistic/chi_square_test/input_goodness_controller.h"

using namespace Escher;

namespace Inference {

GoodnessTableCell::GoodnessTableCell(Responder * parentResponder, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate, SelectableTableViewDelegate * selectableTableViewDelegate, GoodnessTest * test, InputGoodnessController * inputGoodnessController) :
  EditableCategoricalTableCell(parentResponder, this, selectableTableViewDelegate, dynamicSizeTableViewDataSourceDelegate, test),
  DynamicCellsDataSource<Escher::EvenOddEditableTextCell, k_inputGoodnessTableNumberOfReusableCells>(this),
  m_inputGoodnessController(inputGoodnessController)
{
  m_header[0].setMessage(I18n::Message::Observed);
  m_header[1].setMessage(I18n::Message::Expected);
  for (int i = 0; i < GoodnessTest::k_maxNumberOfColumns; i++) {
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
  willDisplayValueCellAtLocation(myCell->editableTextCell()->textField(), myCell, i, j - 1, m_statistic);
}

bool GoodnessTableCell::recomputeDimensions(Chi2Test * s) {
  // Update degree of freedom if Number of non-empty rows changed
  if (EditableCategoricalTableCell::recomputeDimensions(s)) {
    int newDegreeOfFreedom = statistic()->computeDegreesOfFreedom();
    statistic()->setDegreeOfFreedom(newDegreeOfFreedom);
    m_inputGoodnessController->updateDegreeOfFreedomCell();
    return true;
  }
  return false;
}

}  // namespace Inference
