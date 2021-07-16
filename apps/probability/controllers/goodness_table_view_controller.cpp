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

  int index = 2 * (m_seletableTableView->selectedRow() - 1) +
              m_seletableTableView->selectedColumn();
  m_statistic->setParamAtIndex(index, p);
  if (m_seletableTableView->selectedRow() == m_dataSource->numberOfRows() - 1 &&
      m_dataSource->numberOfRows() < GoodnessStatistic::k_maxNumberOfRows) {
    m_dataSource->addRow();
  }
  m_seletableTableView->selectCellAtLocation(m_seletableTableView->selectedColumn(),
                                             m_seletableTableView->selectedRow() + 1);
  m_seletableTableView->reloadData(false);  // TODO why needed ?
  return true;
}

}  // namespace Probability
