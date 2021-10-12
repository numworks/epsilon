#include "homogeneity_table_view_controller.h"
#include "probability/app.h"

namespace Probability {

HomogeneityTableViewController::HomogeneityTableViewController(
    Escher::Responder * parent,
    HomogeneityStatistic * statistic,
    DynamicSizeTableViewDataSourceDelegate * dataSourceDelegate,
    Escher::SelectableTableViewDelegate * tableDelegate) :
      TableViewController(parent),
      m_innerTableData(this, statistic, dataSourceDelegate),
      m_tableData(&m_innerTableData, tableDelegate),
      m_table(this, &m_tableData, &m_selectionDataSource, &m_tableData),
      m_statistic(statistic) {
  m_selectionDataSource.selectColumn(-1);
}

void HomogeneityTableViewController::initCell(void * cell, int index) {
  EvenOddEditableTextCell * c = static_cast<EvenOddEditableTextCell *>(cell);
  c->setParentResponder(&m_table);
  c->editableTextCell()->textField()->setDelegates(Probability::App::app(), this);
  c->setFont(KDFont::SmallFont);
}

void HomogeneityTableViewController::didBecomeFirstResponder() {
  if (m_table.selectedColumn() <= 0) {
    m_table.selectColumn(1);
  }
  TableViewController::didBecomeFirstResponder();
}

bool HomogeneityTableViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace) {
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
  if (!m_statistic->isValidParamAtPosition(row, column, p)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_statistic->setParameterAtPosition(row - 1, column - 1, p);

  m_table.deselectTable();
  // Add row or column
  if ((row == m_innerTableData.numberOfRows() &&
       m_innerTableData.numberOfRows() < HomogeneityStatistic::k_maxNumberOfRows) ||
      (column == m_innerTableData.numberOfColumns() &&
       m_innerTableData.numberOfColumns() < HomogeneityStatistic::k_maxNumberOfColumns)) {
    m_innerTableData.recomputeDimensions();
  }
  m_table.reloadCellAtLocation(column, row);
  moveSelectionForEvent(event, &row, &column);
  m_table.selectCellAtClippedLocation(column, row);
  return true;
}

void HomogeneityTableViewController::deleteSelectedValue() {
  int row = m_table.selectedRow(), col = m_table.selectedColumn();
  m_statistic->setParameterAtPosition(row - 1, col - 1, HomogeneityStatistic::k_undefinedValue);
  m_statistic->recomputeData();
  m_table.reloadCellAtLocation(col, row);

  // Delete last row / column
  m_table.deselectTable();
  m_innerTableData.recomputeDimensions();
  m_table.selectCellAtClippedLocation(col, row, false);
}

}  // namespace Probability
