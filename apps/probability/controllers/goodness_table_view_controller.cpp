#include "goodness_table_view_controller.h"

namespace Probability {

GoodnessTableViewController::GoodnessTableViewController(Escher::Responder * parent,
                                                         GoodnessStatistic * statistic,
                                                         InputGoodnessTableView * dataSource) :
    TableViewController(parent, dataSource), m_statistic(statistic), m_dataSource(dataSource) {
}

bool GoodnessTableViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace &&
      m_seletableTableView->selectedRow() == m_dataSource->numberOfRows() - 1) {
    m_dataSource->deleteLastRow();
    return true;
  }
  return false;
}

bool GoodnessTableViewController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                            const char * text,
                                                            Ion::Events::Event event) {
  float p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, p, false, false)) {
    return false;
  }

  int selectedColumn = m_seletableTableView->selectedColumn();
  int index = 2 * (m_seletableTableView->selectedRow() - 1) + selectedColumn;
  m_statistic->setParamAtIndex(index, p);
  if (m_seletableTableView->selectedRow() == m_dataSource->numberOfRows() - 1 &&
      m_dataSource->numberOfRows() < GoodnessStatistic::k_maxNumberOfRows) {
    m_dataSource->addRow();
  }
  // Select new column or jump to new row
  m_seletableTableView->selectCellAtLocation(
      1 - selectedColumn,
      m_seletableTableView->selectedRow() + (selectedColumn == 1));
  m_seletableTableView->reloadData(false);  // TODO why needed ?
  return true;
}

}  // namespace Probability
