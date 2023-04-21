#include "categorical_table_cell.h"

#include <inference/models/statistic/slope_t_interval.h>
#include <inference/models/statistic/slope_t_test.h>

#include "categorical_controller.h"
#include "inference/app.h"

using namespace Escher;

namespace Inference {

/* CategoricalTableCell */

CategoricalTableCell::CategoricalTableCell(
    Escher::Responder *parentResponder, Escher::TableViewDataSource *dataSource)
    : Escher::Responder(parentResponder),
      m_selectableTableView(this, dataSource, this, this) {
  m_selectableTableView.setBackgroundColor(Escher::Palette::WallScreenDark);
  m_selectableTableView.hideScrollBars();
}

void CategoricalTableCell::didBecomeFirstResponder() {
  assert(0 <= selectedColumn() &&
         selectedColumn() < tableViewDataSource()->numberOfColumns());
  assert(selectedRow() < tableViewDataSource()->numberOfRows());
  if (selectedRow() < 0) {
    selectRow(1);
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

void CategoricalTableCell::tableViewDidChangeSelection(
    SelectableTableView *t, int previousSelectedCol, int previousSelectedRow,
    bool withinTemporarySelection) {
  assert(t == &m_selectableTableView);
  if (withinTemporarySelection || previousSelectedRow == t->selectedRow()) {
    return;
  }
  KDCoordinate verticalOffset = m_selectableTableView.contentOffset().y();
  KDCoordinate tableCellRequiredHeight =
      m_selectableTableView.minimalSizeForOptimalDisplay().height();
  KDCoordinate displayedHeight =
      categoricalController()->view()->bounds().height();
  KDCoordinate givenHeight;
  if (verticalOffset + displayedHeight < tableCellRequiredHeight) {
    /* We need to clip the size of the CategoricalTableCell to force it to
     * scroll */
    givenHeight = displayedHeight;
  } else {
    /* We need to enlarge the size of the CategoricalTableCell to authorize it
     * to scroll downer than its own height */
    givenHeight = tableCellRequiredHeight - verticalOffset;
  }
  m_selectableTableView.setSize(
      KDSize(categoricalController()->view()->bounds().width(), givenHeight));
  m_selectableTableView.scrollToCell(t->selectedColumn(), t->selectedRow());
  if (m_selectableTableView.contentOffset().y() != verticalOffset) {
    // Relayout the whole Categorical table if the scroll change
    categoricalController()->selectableListView()->reloadData(false);
  }
}

void CategoricalTableCell::layoutSubviews(bool force) {
  /* We let an empty border as it will be drawn by the next cell (thanks to the
   * cell overlap) */
  setChildFrame(&m_selectableTableView,
                KDRect(0, 0, bounds().width(),
                       bounds().height() - Metric::CellSeparatorThickness),
                force);
}

/* EditableCategoricalTableCell */

EditableCategoricalTableCell::EditableCategoricalTableCell(
    Escher::Responder *parentResponder, Escher::TableViewDataSource *dataSource,
    DynamicSizeTableViewDataSourceDelegate *dynamicSizeTableViewDelegate,
    Statistic *statistic)
    : CategoricalTableCell(parentResponder, dataSource),
      DynamicSizeTableViewDataSource(dynamicSizeTableViewDelegate),
      m_statistic(statistic) {
  m_selectableTableView.setBottomMargin(k_bottomMargin);
}

bool EditableCategoricalTableCell::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE ||
         (event == Ion::Events::Right &&
          m_selectableTableView.selectedColumn() <
              tableViewDataSource()->numberOfColumns() - 1) ||
         (event == Ion::Events::Left &&
          m_selectableTableView.selectedColumn() > 0) ||
         (event == Ion::Events::Down &&
          m_selectableTableView.selectedRow() <
              tableViewDataSource()->numberOfRows()) ||
         (event == Ion::Events::Up && m_selectableTableView.selectedRow() > 0);
}

bool EditableCategoricalTableCell::textFieldDidFinishEditing(
    Escher::AbstractTextField *textField, const char *text,
    Ion::Events::Event event) {
  double p = textFieldDelegateApp()->parseInputtedFloatValue<double>(text);
  if (textFieldDelegateApp()->hasUndefinedValue(p, false, false)) {
    return false;
  }
  int row = m_selectableTableView.selectedRow(),
      column = m_selectableTableView.selectedColumn();
  if (!tableModel()->authorizedParameterAtPosition(
          p, relativeRowIndex(row), relativeColumnIndex(column))) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  tableModel()->setParameterAtPosition(p, relativeRowIndex(row),
                                       relativeColumnIndex(column));

  m_selectableTableView.deselectTable(true);
  // Add row or column
  if ((row == tableViewDataSource()->numberOfRows() - 1 &&
       relativeRowIndex(tableViewDataSource()->numberOfRows()) <
           tableModel()->maxNumberOfRows()) ||
      (column == tableViewDataSource()->numberOfColumns() - 1 &&
       relativeColumnIndex(tableViewDataSource()->numberOfColumns()) <
           tableModel()->maxNumberOfColumns())) {
    recomputeDimensions();
  }
  m_selectableTableView.reloadCellAtLocation(column, row);
  m_selectableTableView.selectCellAtClippedLocation(column, row);
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    event = Ion::Events::Down;
  }
  m_selectableTableView.handleEvent(event);
  return true;
}

bool EditableCategoricalTableCell::handleEvent(Ion::Events::Event event) {
  int column = m_selectableTableView.selectedColumn();
  int row = m_selectableTableView.selectedRow();
  int cellType = tableViewDataSource()->typeAtLocation(column, row);
  if (event == Ion::Events::Backspace &&
      cellType == CategoricalTableViewDataSource::k_typeOfInnerCells) {
    deleteSelectedValue();
    return true;
  } else if (event == Ion::Events::Backspace || event == Ion::Events::Clear) {
    presentClearSelectedColumnPopupIfClearable();
    return true;
  }
  return CategoricalTableCell::handleEvent(event);
}

void EditableCategoricalTableCell::initCell(InferenceEvenOddEditableCell,
                                            void *cell, int index) {
  InferenceEvenOddEditableCell *c =
      static_cast<InferenceEvenOddEditableCell *>(cell);
  c->setParentResponder(&m_selectableTableView);
  c->editableTextCell()->textField()->setDelegates(App::app(), this);
}

bool EditableCategoricalTableCell::deleteSelectedValue() {
  int row = m_selectableTableView.selectedRow(),
      col = m_selectableTableView.selectedColumn();
  assert(relativeRowIndex(row) >= 0 && relativeColumnIndex(col) >= 0);
  // Remove value
  bool shouldDeleteRowOrCol = tableModel()->deleteParameterAtPosition(
      relativeRowIndex(row), relativeColumnIndex(col));
  if (!shouldDeleteRowOrCol) {
    // Only one cell needs to reload.
    assert(row < tableViewDataSource()->numberOfRows() &&
           col < tableViewDataSource()->numberOfColumns());
    m_selectableTableView.reloadCellAtLocation(col, row);
    return false;
  } else {
    // A row and/or column has been deleted, we should recompute data
    tableModel()->recomputeData();
    /* Due to an initial number of rows/ols of 2, we cannot ensure that at most
     * one row and one col have been deleted here */
    m_selectableTableView.deselectTable();
    if (!recomputeDimensions()) {
      /* A row has been deleted, but size didn't change, meaning the number of
       * non-empty rows was k_maxNumberOfRows. However, recomputeData may have
       * moved up multiple cells, m_inputTableView should be reloaded. */
      m_selectableTableView.reloadData(false);
    }
    selectableTableView()->selectCellAtClippedLocation(col, row, true);
    return true;
  }
}

int EditableCategoricalTableCell::numberOfElementsInColumn(int column) const {
  int n = constTableModel()->maxNumberOfRows();
  column = relativeColumnIndex(column);
  int res = 0;
  for (int row = 0; row < n; row++) {
    res += std::isfinite(constTableModel()->parameterAtPosition(row, column));
  }
  return res;
}

void EditableCategoricalTableCell::clearSelectedColumn() {
  int column = m_selectableTableView.selectedColumn();
  tableModel()->deleteParametersInColumn(relativeColumnIndex(column));
  tableModel()->recomputeData();
  selectableTableView()->selectCellAtClippedLocation(column, 1, false);
  if (!recomputeDimensions()) {
    m_selectableTableView.reloadData(false);
  }
}

bool EditableCategoricalTableCell::recomputeDimensions() {
  // Return true if size changed
  Chi2Test::Index2D dimensions = tableModel()->computeDimensions();
  return didChangeSize(dimensions.row, dimensions.col);
}

Table *EditableCategoricalTableCell::tableModel() {
  if (m_statistic->subApp() == Statistic::SubApp::Test) {
    if (m_statistic->significanceTestType() == SignificanceTestType::Slope) {
      return static_cast<SlopeTTest *>(m_statistic);
    } else {
      assert(m_statistic->significanceTestType() ==
             SignificanceTestType::Categorical);
      return static_cast<Chi2Test *>(m_statistic);
    }
  }
  assert(m_statistic->subApp() == Statistic::SubApp::Interval);
  assert(m_statistic->significanceTestType() == SignificanceTestType::Slope);
  return static_cast<SlopeTInterval *>(m_statistic);
}

/* DoubleColumnTableCell */

DoubleColumnTableCell::DoubleColumnTableCell(
    Escher::Responder *parentResponder,
    DynamicSizeTableViewDataSourceDelegate
        *dynamicSizeTableViewDataSourceDelegate,
    Statistic *statistic)
    : EditableCategoricalTableCell(parentResponder, this,
                                   dynamicSizeTableViewDataSourceDelegate,
                                   statistic),
      DynamicCellsDataSource<InferenceEvenOddEditableCell,
                             k_doubleColumnTableNumberOfReusableCells>(this) {}

int DoubleColumnTableCell::reusableCellCount(int type) {
  if (type == k_typeOfHeaderCells) {
    return k_maxNumberOfColumns;
  }
  return k_maxNumberOfReusableRows * k_maxNumberOfColumns;
}

HighlightCell *DoubleColumnTableCell::reusableCell(int i, int type) {
  assert(i < reusableCellCount(type));
  if (type == k_typeOfHeaderCells) {
    assert(i < 2);
    return headerCell(i);
  }
  return cell(i);
}

void DoubleColumnTableCell::willDisplayCellAtLocation(
    Escher::HighlightCell *cell, int i, int j) {
  if (j == 0) {  // Header
    return;
  }
  InferenceEvenOddEditableCell *myCell =
      static_cast<InferenceEvenOddEditableCell *>(cell);
  willDisplayValueCellAtLocation(myCell->editableTextCell()->textField(),
                                 myCell, i, j - 1, tableModel());
}

}  // namespace Inference
