#include "categorical_table_cell.h"
#include "../app.h"

using namespace Escher;

namespace Probability {

/* CategoricalTableCell */

CategoricalTableCell::CategoricalTableCell(Escher::Responder * parentResponder, Escher::TableViewDataSource * dataSource, Escher::SelectableTableViewDelegate * selectableTableViewDelegate) :
  Escher::Responder(parentResponder),
  m_selectableTableView(this, dataSource, this, selectableTableViewDelegate)
{
  m_selectableTableView.setBackgroundColor(Escher::Palette::WallScreenDark);
  m_selectableTableView.setDecoratorType(Escher::ScrollView::Decorator::Type::None);
}

void CategoricalTableCell::didBecomeFirstResponder() {
  if (selectableTableView()->selectedRow() < 0) {
    selectableTableView()->selectRow(1);
  }
  Escher::Container::activeApp()->setFirstResponder(selectableTableView());
}

bool CategoricalTableCell::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::Left) {
    // Catch left event to avoid popping controller from StackViewController
    return true;
  }
  return false;
}

void CategoricalTableCell::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw only part borders between cells (in *)
  // ┌─────┐*┌─────┐
  // │cell │*│cell │
  // └─────┘*└─────┘
  // ********
  // ┌─────┐
  // │cell │
  // └─────┘

  KDPoint offset = KDPoint(m_selectableTableView.leftMargin(), m_selectableTableView.topMargin()).translatedBy(m_selectableTableView.contentOffset().opposite());
  TableViewDataSource * data = const_cast<CategoricalTableCell *>(this)->tableViewDataSource();

  for (int row = 0; row < data->numberOfRows() - 1; row++) {
    KDRect horizontalBorder = KDRect(0,
        data->cumulatedHeightFromIndex(row) + data->rowHeight(row),
                                     bounds().width(),
                                     data->cumulatedHeightFromIndex(row + 1) -
                                         data->cumulatedHeightFromIndex(row) -
                                         data->rowHeight(row));
    ctx->fillRect(horizontalBorder.translatedBy(offset), m_selectableTableView.backgroundColor());
  }

  for (int column = 0; column < data->numberOfColumns() - 1; column++) {
    KDRect verticalBorder = KDRect(data->cumulatedWidthFromIndex(column) + data->columnWidth(column),
                                   0,
                                   data->cumulatedWidthFromIndex(column + 1) -
                                       data->cumulatedWidthFromIndex(column) -
                                       data->columnWidth(column),
                                   bounds().height());
    ctx->fillRect(verticalBorder.translatedBy(offset), m_selectableTableView.backgroundColor());
  }
}

void CategoricalTableCell::layoutSubviews(bool force) {
  m_selectableTableView.setFrame(bounds(), force);
}

/* EditableCategoricalTableCell */

EditableCategoricalTableCell::EditableCategoricalTableCell(Escher::Responder * parentResponder, Escher::TableViewDataSource * dataSource, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDelegate, Chi2Test * chi2Test) :
  CategoricalTableCell(parentResponder, dataSource, selectableTableViewDelegate),
  DynamicSizeTableViewDataSource(dynamicSizeTableViewDelegate),
  m_statistic(chi2Test)
{
  m_selectableTableView.setMargins(0, 0, k_marginVertical, 0);
}

bool EditableCategoricalTableCell::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE ||
         (event == Ion::Events::Right &&
          m_selectableTableView.selectedColumn() < tableViewDataSource()->numberOfColumns() - 1) ||
         (event == Ion::Events::Left && m_selectableTableView.selectedColumn() > 0) ||
         (event == Ion::Events::Down &&
          m_selectableTableView.selectedRow() < tableViewDataSource()->numberOfRows() - 1) ||
         (event == Ion::Events::Up && m_selectableTableView.selectedRow() > 0);
}

bool EditableCategoricalTableCell::textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) {
  double p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &p, false, false)) {
    return false;
  }
  int row = m_selectableTableView.selectedRow(), column = m_selectableTableView.selectedColumn();
  if (!m_statistic->authorizedParameterAtPosition(p, relativeRowIndex(row), relativeColumnIndex(column))) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_statistic->setParameterAtPosition(p,  relativeRowIndex(row), relativeColumnIndex(column));

  m_selectableTableView.deselectTable();
  // Add row or column
  if ((row == tableViewDataSource()->numberOfRows() - 1 && relativeRowIndex(tableViewDataSource()->numberOfRows()) < m_statistic->maxNumberOfRows()) ||
      (column == tableViewDataSource()->numberOfColumns() && relativeColumnIndex(tableViewDataSource()->numberOfColumns() < m_statistic->maxNumberOfColumns()))) {
    recomputeDimensions(m_statistic);
  }
  m_selectableTableView.reloadCellAtLocation(column, row);
  m_selectableTableView.handleEvent(event);
  m_selectableTableView.selectCellAtClippedLocation(column, row);
  return true;
}

bool EditableCategoricalTableCell::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace && tableViewDataSource()->typeAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()) == CategoricalTableViewDataSource::k_typeOfInnerCells) {
    deleteSelectedValue();
    return true;
  }
  return CategoricalTableCell::handleEvent(event);
}

void EditableCategoricalTableCell::initCell(EvenOddEditableTextCell, void * cell, int index) {
  EvenOddEditableTextCell * c = static_cast<EvenOddEditableTextCell *>(cell);
  c->setParentResponder(&m_selectableTableView);
  c->editableTextCell()->textField()->setDelegates(Probability::App::app(), this);
  c->setFont(KDFont::SmallFont);
}

bool EditableCategoricalTableCell::deleteSelectedValue() {
  int row = m_selectableTableView.selectedRow(), col = m_selectableTableView.selectedColumn();
  assert(relativeRowIndex(row) >= 0 && relativeColumnIndex(col) >= 0);
  // Remove value
  bool shouldDeleteRowOrCol = m_statistic->deleteParameterAtPosition(relativeRowIndex(row), relativeColumnIndex(col));
  if (!shouldDeleteRowOrCol) {
    // Only one cell needs to reload.
    assert(row < tableViewDataSource()->numberOfRows() && col < tableViewDataSource()->numberOfColumns());
    m_selectableTableView.reloadCellAtLocation(col, row);
    return false;
  } else {
    // A row and/or column has been deleted, we should recompute data
    m_statistic->recomputeData();
    /* Due to an initial number of rows/ols of 2, we cannot ensure that at most
     * one row and one col have been deleted here */
    m_selectableTableView.deselectTable();
    if (!recomputeDimensions(m_statistic)) {
      /* A row has been deleted, but size didn't change, meaning the number of
       * non-empty rows was k_maxNumberOfRows. However, recomputeData may have
       * moved up multiple cells, m_inputTableView should be reloaded. */
      m_selectableTableView.reloadData(false);
    }
    tableView()->selectCellAtClippedLocation(col, row, false);
    return true;
  }
}

bool EditableCategoricalTableCell::recomputeDimensions(Chi2Test * test) {
  // Return true if size changed
  Chi2Test::Index2D dimensions = test->computeDimensions();
  return didChangeSize(dimensions.row, dimensions.col);
}

}  // namespace Probability
