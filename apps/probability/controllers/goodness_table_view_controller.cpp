#include "goodness_table_view_controller.h"

#include <algorithm>

namespace Probability {

GoodnessTableViewController::GoodnessTableViewController(
    Escher::Responder * parent,
    GoodnessStatistic * statistic,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    DynamicTableViewDataSourceDelegate * delegate,
    Escher::SelectableTableViewDelegate * scrollDelegate) :
    TableViewController(parent),
    m_statistic(statistic),
    m_inputTableView(this, inputEventHandlerDelegate, statistic, this, delegate, scrollDelegate) {
}

void GoodnessTableViewController::didBecomeFirstResponder() {
  m_inputTableView.recomputeNumberOfRows();
  TableViewController::didBecomeFirstResponder();
}

bool GoodnessTableViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace) {
    // Remove value
    int row = m_inputTableView.selectedRow(), col = m_inputTableView.selectedColumn();
    m_statistic->setParamAtIndex(2 * (row - 1) + col, GoodnessStatistic::k_undefinedValue);

    // Delete last row if needed
    if (row == m_inputTableView.numberOfRows() - 2 &&
        m_inputTableView.numberOfRows() > InputGoodnessTableView::k_minimumNumberOfRows &&
        std::isnan(m_statistic->paramAtIndex(2 * (row - 1) + (1 - col)))) {
      // Unhighlight selected cell in case it disappears after the row is removed
      m_inputTableView.unhighlightSelectedCell();
      m_inputTableView.recomputeNumberOfRows();
      m_inputTableView.selectCellAtLocation(col,
                                            std::min(row, m_inputTableView.numberOfRows() - 1));
    }
    m_inputTableView.reloadCellAtLocation(col, row);
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

  int selectedColumn = m_inputTableView.selectedColumn();
  int index = 2 * (m_inputTableView.selectedRow() - 1) + selectedColumn;
  m_statistic->setParamAtIndex(index, p);
  if (m_inputTableView.selectedRow() == m_inputTableView.numberOfRows() - 1 &&
      m_inputTableView.numberOfRows() < GoodnessStatistic::k_maxNumberOfRows) {
    m_inputTableView.recomputeNumberOfRows();
  }
  // Select new column or jump to new row
  m_inputTableView.selectCellAtLocation(1 - selectedColumn,
                                        m_inputTableView.selectedRow() + (selectedColumn == 1));
  m_inputTableView.reloadData(false);  // TODO why needed ?
  return true;
}

}  // namespace Probability
