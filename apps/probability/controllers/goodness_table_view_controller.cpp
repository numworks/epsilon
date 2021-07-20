#include "goodness_table_view_controller.h"

#include <algorithm>

namespace Probability {

GoodnessTableViewController::GoodnessTableViewController(Escher::Responder * parent,
                                                         GoodnessStatistic * statistic,
                                                         InputGoodnessTableView * dataSource) :
    TableViewController(parent, dataSource), m_statistic(statistic), m_dataSource(dataSource) {
}

bool GoodnessTableViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace) {
    // Remove value
    int row = m_dataSource->selectedRow(), col = m_dataSource->selectedColumn();
    m_statistic->setParamAtIndex(2 * (row - 1) + col, GoodnessStatistic::k_undefinedValue);

    // Delete last row if needed
    if (row == m_dataSource->numberOfRows() - 2 &&
        m_dataSource->numberOfRows() > InputGoodnessTableView::k_minimumNumberOfRows &&
        std::isnan(m_statistic->paramAtIndex(2 * (row - 1) + (1 - col)))) {
      // Unhighlight selected cell in case it disappears after the row is removed
      m_dataSource->unhighlightSelectedCell();
      m_dataSource->recomputeNumberOfRows();
      m_dataSource->selectCellAtLocation(col, std::min(row, m_dataSource->numberOfRows() - 1));
    }
    m_dataSource->reloadCellAtLocation(col, row);
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
    m_dataSource->recomputeNumberOfRows();
  }
  // Select new column or jump to new row
  m_seletableTableView->selectCellAtLocation(
      1 - selectedColumn,
      m_seletableTableView->selectedRow() + (selectedColumn == 1));
  m_seletableTableView->reloadData(false);  // TODO why needed ?
  return true;
}

}  // namespace Probability
