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
    // Delete value
    int row = m_seletableTableView->selectedRow() - 1,
        col = m_seletableTableView->selectedColumn() - 1;
    m_statistic->setParameterAtPosition(row, col, HomogeneityStatistic::k_undefinedValue);

    // Delete last row / column
    m_seletableTableView->deselectTable();
    if (row == m_dataSource->numberOfRows() - 2) {
      m_dataSource->recomputeDimensions();
    }
    if (col == m_dataSource->numberOfColumns() - 2) {
      m_dataSource->recomputeDimensions();
    }
    m_seletableTableView->selectCellAtClippedLocation(col + 1, row + 1, false);
    m_seletableTableView->reloadCellAtLocation(col + 1, row + 1);
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
  int row = m_seletableTableView->selectedRow(), column = m_seletableTableView->selectedColumn();
  m_statistic->setParameterAtPosition(row - 1, column - 1, p);
  
  m_seletableTableView->deselectTable();
  // Add row
  if (row == m_dataSource->numberOfRows() &&
      m_dataSource->numberOfRows() < HomogeneityStatistic::k_maxNumberOfRows) {
    m_dataSource->recomputeDimensions();
  }
  // Add column
  if (column == m_dataSource->numberOfColumns() &&
      m_dataSource->numberOfColumns() < HomogeneityStatistic::k_maxNumberOfColumns) {
    m_dataSource->recomputeDimensions();
  }

  m_seletableTableView->selectCellAtLocation(column, row + 1);
  m_seletableTableView->reloadData(false);  // TODO why needed ?
  return true;
}

}  // namespace Probability
