#include "goodness_table_view_controller.h"
#include "probability/app.h"

#include <algorithm>

namespace Probability {

GoodnessTableViewController::GoodnessTableViewController(
    Escher::Responder * parent,
    GoodnessStatistic * statistic,
    DynamicSizeTableViewDataSourceDelegate * delegate,
    Escher::SelectableTableViewDelegate * scrollDelegate) :
      TableViewController(parent),
      m_statistic(statistic),
      m_inputTableView(this, statistic, this, this, delegate, scrollDelegate) {
}

void GoodnessTableViewController::didBecomeFirstResponder() {
  m_inputTableView.recomputeNumberOfRows();
  TableViewController::didBecomeFirstResponder();
}

bool GoodnessTableViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace) {
    deleteSelectedValue();
    return true;
  }
  return TableViewController::handleEvent(event);
}

bool GoodnessTableViewController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                            const char * text,
                                                            Ion::Events::Event event) {
  // TODO: factorize with InputCategoricalData & HomogeneityTableViewController
  float p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &p, false, false)) {
    return false;
  }
  int selectedColumn = m_inputTableView.selectedColumn();
  int selectedRow = m_inputTableView.selectedRow();
  if (!m_statistic->isValidParamAtLocation(selectedRow - 1, selectedColumn, p)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_statistic->setParamAtLocation(selectedRow - 1, selectedColumn, p);
  if (m_inputTableView.selectedRow() == m_inputTableView.numberOfRows() - 1 &&
      m_inputTableView.numberOfRows() <= GoodnessStatistic::k_maxNumberOfRows) {
    m_inputTableView.recomputeNumberOfRows();
  }

  m_inputTableView.reloadCellAtLocation(selectedColumn, selectedRow);
  // Select correct cell
  moveSelectionForEvent(event, &selectedRow, &selectedColumn);
  m_inputTableView.selectCellAtClippedLocation(selectedColumn, selectedRow);
  return true;
}

void GoodnessTableViewController::deleteSelectedValue() {
  // Remove value
  int row = m_inputTableView.selectedRow(), col = m_inputTableView.selectedColumn();
  m_statistic->setParamAtLocation(row - 1, col, GoodnessStatistic::k_undefinedValue);

  // Delete row if needed
  // Unhighlight selected cell in case it disappears after the row is removed
  m_inputTableView.unhighlightSelectedCell();
  m_statistic->recomputeData();
  m_inputTableView.recomputeNumberOfRows();
  m_inputTableView.selectCellAtLocation(col, std::min(row, m_inputTableView.numberOfRows() - 1));
}

void GoodnessTableViewController::initCell(void * cell, int index) {
  Escher::EvenOddEditableTextCell * c = static_cast<Escher::EvenOddEditableTextCell *>(cell);
  c->setParentResponder(&m_inputTableView);
  c->editableTextCell()->textField()->setDelegates(Probability::App::app(), this);
  c->setEven((index / 2) % 2 == 1);
  c->setFont(KDFont::SmallFont);
}

}  // namespace Probability
