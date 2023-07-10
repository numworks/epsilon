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
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool CategoricalTableCell::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::Left) {
    // Catch left event to avoid popping controller from StackViewController
    return true;
  }
  return false;
}

void CategoricalTableCell::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView *t, int previousSelectedCol, int previousSelectedRow,
    KDPoint previousOffset, bool withinTemporarySelection) {
  assert(t == &m_selectableTableView);
  if (!withinTemporarySelection && previousOffset != t->contentOffset()) {
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

/* InputCategoricalTableCell */

InputCategoricalTableCell::InputCategoricalTableCell(
    Escher::Responder *parentResponder, Escher::TableViewDataSource *dataSource,
    Statistic *statistic)
    : CategoricalTableCell(parentResponder, dataSource),
      m_statistic(statistic),
      m_numberOfRows(0),
      m_numberOfColumns(0) {
  m_selectableTableView.setBottomMargin(k_bottomMargin);
}

bool InputCategoricalTableCell::textFieldShouldFinishEditing(
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

bool InputCategoricalTableCell::textFieldDidFinishEditing(
    Escher::AbstractTextField *textField, const char *text,
    Ion::Events::Event event) {
  double p = textFieldDelegateApp()->parseInputtedFloatValue<double>(text);
  if (textFieldDelegateApp()->hasUndefinedValue(p, false, false)) {
    return false;
  }
  int row = m_selectableTableView.selectedRow(),
      column = m_selectableTableView.selectedColumn();
  if (!tableModel()->authorizedParameterAtPosition(p, relativeRow(row),
                                                   relativeColumn(column))) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  tableModel()->setParameterAtPosition(p, relativeRow(row),
                                       relativeColumn(column));

  m_selectableTableView.deselectTable(true);
  // Add row or column
  if ((row == tableViewDataSource()->numberOfRows() - 1 &&
       relativeRow(tableViewDataSource()->numberOfRows()) <
           tableModel()->maxNumberOfRows()) ||
      (column == tableViewDataSource()->numberOfColumns() - 1 &&
       relativeColumn(tableViewDataSource()->numberOfColumns()) <
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

bool InputCategoricalTableCell::handleEvent(Ion::Events::Event event) {
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

void InputCategoricalTableCell::initCell(InferenceEvenOddEditableCell,
                                         void *cell, int index) {
  InferenceEvenOddEditableCell *c =
      static_cast<InferenceEvenOddEditableCell *>(cell);
  c->setParentResponder(&m_selectableTableView);
  c->editableTextCell()->textField()->setDelegates(App::app(), this);
}

bool InputCategoricalTableCell::deleteSelectedValue() {
  int row = m_selectableTableView.selectedRow(),
      col = m_selectableTableView.selectedColumn();
  assert(relativeRow(row) >= 0 && relativeColumn(col) >= 0);
  // Remove value
  bool shouldDeleteRowOrCol = tableModel()->deleteParameterAtPosition(
      relativeRow(row), relativeColumn(col));
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
    m_selectableTableView.selectCellAtClippedLocation(col, row, true);
    return true;
  }
}

int InputCategoricalTableCell::numberOfElementsInColumn(int column) const {
  int n = constTableModel()->maxNumberOfRows();
  column = relativeColumn(column);
  int res = 0;
  for (int row = 0; row < n; row++) {
    res += std::isfinite(constTableModel()->parameterAtPosition(row, column));
  }
  return res;
}

void InputCategoricalTableCell::clearSelectedColumn() {
  int column = m_selectableTableView.selectedColumn();
  tableModel()->deleteParametersInColumn(relativeColumn(column));
  tableModel()->recomputeData();
  m_selectableTableView.deselectTable();
  m_selectableTableView.setContentOffset(KDPointZero);
  if (!recomputeDimensions()) {
    m_selectableTableView.reloadData(false);
    categoricalController()->selectableListView()->reloadData(false);
  }
  m_selectableTableView.selectCellAtClippedLocation(column, 1, false);
}

bool InputCategoricalTableCell::recomputeDimensions() {
  Chi2Test::Index2D dimensions = tableModel()->computeDimensions();
  if (m_numberOfRows == dimensions.row && m_numberOfColumns == dimensions.col) {
    return false;
  }
  m_numberOfRows = dimensions.row;
  m_numberOfColumns = dimensions.col;
  /* Relayout when inner table changes size. We need to reload the table because
   * its width might change but it won't relayout as its frame isn't changed by
   * the InputCategoricalController */
  m_selectableTableView.reloadData(false);
  categoricalController()->selectableListView()->reloadData(false);
  return true;
}

Table *InputCategoricalTableCell::tableModel() {
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

DoubleColumnTableCell::DoubleColumnTableCell(Escher::Responder *parentResponder,
                                             Statistic *statistic)
    : InputCategoricalTableCell(parentResponder, this, statistic),
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

void DoubleColumnTableCell::fillCellForLocation(Escher::HighlightCell *cell,
                                                int column, int row) {
  if (row == 0) {  // Header
    return;
  }
  InferenceEvenOddEditableCell *myCell =
      static_cast<InferenceEvenOddEditableCell *>(cell);
  willDisplayValueCellAtLocation(myCell->editableTextCell()->textField(),
                                 myCell, column, row - 1, tableModel());
}

}  // namespace Inference
