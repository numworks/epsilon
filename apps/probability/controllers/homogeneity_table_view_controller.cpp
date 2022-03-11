#include "homogeneity_table_view_controller.h"
#include "probability/app.h"

namespace Probability {

HomogeneityTableViewController::HomogeneityTableViewController(
    Escher::Responder * parent,
    HomogeneityTest * statistic,
    DynamicSizeTableViewDataSourceDelegate * dataSourceDelegate,
    Escher::SelectableTableViewDelegate * tableDelegate) :
      TableViewController(parent),
      m_tableData(tableDelegate, this, this, dataSourceDelegate, statistic),
      m_table(this, &m_tableData, &m_selectionDataSource, &m_tableData),
      m_statistic(statistic) {
  m_selectionDataSource.selectColumn(-1);
  m_selectionDataSource.setScrollViewDelegate(&m_table);
}

void HomogeneityTableViewController::initCell(EvenOddEditableTextCell, void * cell, int index) {
  EvenOddEditableTextCell * c = static_cast<EvenOddEditableTextCell *>(cell);
  c->setParentResponder(&m_table);
  c->editableTextCell()->textField()->setDelegates(Probability::App::app(), this);
  c->setFont(KDFont::SmallFont);
}

void HomogeneityTableViewController::initCell(EvenOddBufferTextCell, void * cell, int index) {
  static_cast<EvenOddBufferTextCell *>(cell)->setFont(KDFont::SmallFont);
}

void HomogeneityTableViewController::didBecomeFirstResponder() {
  if (m_table.selectedColumn() <= 0) {
    m_table.selectColumn(1);
  }
  TableViewController::didBecomeFirstResponder();
}

bool HomogeneityTableViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace && m_table.selectedRow() > 0 && m_table.selectedColumn() > 0) {
    deleteSelectedValue();
    return true;
  }
  return TableViewController::handleEvent(event);
}

bool HomogeneityTableViewController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                               const char * text,
                                                               Ion::Events::Event event) {
  // TODO: factorize with InputCategoricalData & GoodnessTableViewController
  double p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &p, false, false)) {
    return false;
  }
  int row = m_table.selectedRow(), column = m_table.selectedColumn();
  if (!m_statistic->authorizedParameterAtPosition(p, row - 1, column - 1)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_statistic->setParameterAtPosition(p, row - 1, column - 1);

  m_table.deselectTable();
  // Add row or column
  if ((row == m_tableData.innerNumberOfRows() &&
       m_tableData.innerNumberOfRows() < m_statistic->maxNumberOfRows()) ||
      (column == m_tableData.innerNumberOfColumns() &&
       m_tableData.innerNumberOfColumns() < m_statistic->maxNumberOfColumns())) {
    m_tableData.recomputeDimensions();
  }
  m_table.reloadCellAtLocation(column, row);
  moveSelectionForEvent(event, &row, &column);
  m_table.selectCellAtClippedLocation(column, row);
  return true;
}

// See TODO from GoodnessTableViewController::deleteSelectedValue
void HomogeneityTableViewController::deleteSelectedValue() {
  int row = tableView()->selectedRow(), col = tableView()->selectedColumn();
  assert(row > 0 && col > 0);
  // Remove value
  bool shouldDeleteRowOrCol = m_statistic->deleteParameterAtPosition(row - 1, col - 1);
  if (!shouldDeleteRowOrCol) {
    // Only one cell needs to reload.
    assert(row <=  m_tableData.innerNumberOfRows() && col <= m_tableData.innerNumberOfColumns());
    tableView()->reloadCellAtLocation(col, row);
  } else {
    // A row and/or column has been deleted, we should recompute data
    m_statistic->recomputeData();
    /* Due to an initial number of rows/ols of 2, we cannot ensure that at most
     * one row and one col have been deleted here */
    tableView()->deselectTable();
    if (!m_tableData.recomputeDimensions()) {
      /* A row/col has been deleted, but size didn't change, meaning the number
       * of non-empty rows/cols was at maximum. However, recomputeData may have
       * moved up/left multiple cells, m_inputTableView should be reloaded. */
      tableView()->reloadData(false);
    }
    tableView()->selectCellAtClippedLocation(col, row, false);
  }
}

}  // namespace Probability
