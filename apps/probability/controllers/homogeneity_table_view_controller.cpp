#include "homogeneity_table_view_controller.h"

#include "probability/controllers/input_homogeneity_controller.h"

namespace Probability {

HomogeneityTableViewController::HomogeneityTableViewController(
    Escher::Responder * parent,
    Escher::SelectableTableView * tableView,
    InputHomogeneityDataSource * dataSource,
    HomogeneityStatistic * statistic) :
    TableViewController(parent, tableView),
    m_contentView(tableView),
    m_dataSource(dataSource),
    m_statistic(statistic) {
}

void HomogeneityTableViewController::didBecomeFirstResponder() {
  if (m_seletableTableView->selectedColumn() < 0) {
    m_seletableTableView->selectColumn(1);
  }
  TableViewController::didBecomeFirstResponder();
}

bool HomogeneityTableViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace) {
    // Delete last row / column
    if (m_seletableTableView->selectedRow() == m_dataSource->numberOfRows() - 1) {
      m_dataSource->deleteLastRow();
    }
    if (m_seletableTableView->selectedColumn() == m_dataSource->numberOfColumns() - 1) {
      m_dataSource->deleteLastColumn();
    }
    return true;
  }
  return false;
}

bool HomogeneityTableViewController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                               const char * text,
                                                               Ion::Events::Event event) {
  float p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, p, false, false)) {
    return false;
  }

  m_statistic->setParameterAtPosition(m_seletableTableView->selectedRow() - 1,
                                      m_seletableTableView->selectedColumn() - 1,
                                      p);
  // Add row
  if (m_seletableTableView->selectedRow() == m_dataSource->numberOfRows() &&
      m_dataSource->numberOfRows() < HomogeneityStatistic::k_maxNumberOfRows) {
    m_dataSource->addRow();
  }
  // Add column
  if (m_seletableTableView->selectedColumn() == m_dataSource->numberOfColumns() &&
      m_dataSource->numberOfColumns() < HomogeneityStatistic::k_maxNumberOfColumns) {
    m_dataSource->addColumn();
  }

  m_seletableTableView->selectCellAtLocation(m_seletableTableView->selectedColumn(),
                                             m_seletableTableView->selectedRow() + 1);
  m_seletableTableView->reloadData(false);  // TODO why needed ?
  return true;
}

}  // namespace Probability
