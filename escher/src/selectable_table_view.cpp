#include <escher/selectable_table_view.h>
#include <escher/container.h>
#include <escher/clipboard.h>
#include <escher/metric.h>
#include <assert.h>

namespace Escher {

SelectableTableView::SelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource, SelectableTableViewDelegate * delegate) :
  TableView(dataSource, selectionDataSource),
  Responder(parentResponder),
  m_selectionDataSource(selectionDataSource),
  m_delegate(delegate)
{
  assert(m_selectionDataSource != nullptr);
  setMargins(
    Metric::CommonTopMargin,
    Metric::CommonRightMargin,
    Metric::CommonBottomMargin,
    Metric::CommonLeftMargin
  );
}

HighlightCell * SelectableTableView::selectedCell() {
  if (selectedColumn() < 0 || selectedRow() < 0 || selectedColumn() >= dataSource()->numberOfColumns() || selectedRow() >= dataSource()->numberOfRows()) {
    return nullptr;
  }
  return cellAtLocation(selectedColumn(), selectedRow());
}

bool SelectableTableView::cellIsSelectable(HighlightCell * cell) {
  /* There is 2 cases:
   * - In the case of non reusable cells: all cells are valid pointers but they are not all selectable
   *   --> the condition we want to test is cell->isSelectable()
   * - In the case of reusable cells: visible cells are valid pointers and they are all selectable, and 
   *   other cells are nullptr but they also are selectable. 
   *   --> we don't want to test any condition here, but since the 2 cases are handled together here,
   *   we need to add || !cell in the condition
   * TODO: split the class hierarchy earlier */
  return !cell || cell->isSelectable();
}

int SelectableTableView::firstSelectableRow() {
  if (dataSource()->numberOfRows() == 0) {
    return 0;
  }
  for (int row = 0, end = dataSource()->numberOfRows(); row < end; row++) {
    HighlightCell * cell = cellAtLocation(selectedColumn(), row);
    if (cellIsSelectable(cell)) {
      return row;
    }
  }
  assert(false);
  return -1;
}

int SelectableTableView::indexOfNextSelectableColumnOrRow(int delta, bool row) {
  assert(selectedCell());
  // Let's call our variable cow, as a shortcut for col-or-row
  int cow = row ? selectedRow() : selectedColumn();
  int selectableCow = cow;
  const int step = delta > 0 ? 1 : -1;
  const int lastCow = row ? dataSource()->numberOfRows() - 1 : dataSource()->numberOfColumns() - 1;
  while (delta) {
    cow += step;
    if (cow < 0 || cow > lastCow) {
      return selectableCow;
    }
    HighlightCell * cell = row ? cellAtLocation(selectedColumn(), cow) : cellAtLocation(cow, selectedRow());
    if (cellIsSelectable(cell)) {
      selectableCow = cow;
      delta -= step;
    }
  }
  return cow;
}

int SelectableTableView::indexOfNextSelectableRow(int delta) {
  return indexOfNextSelectableColumnOrRow(delta, true);
}

int SelectableTableView::indexOfNextSelectableColumn(int delta) {
  return indexOfNextSelectableColumnOrRow(delta, false);
}

bool SelectableTableView::selectCellAtLocation(int i, int j, bool setFirstResponder, bool withinTemporarySelection) {
  if (i < 0 || i >= dataSource()->numberOfColumns()) {
    return false;
  }
  if (j < 0 || j >= dataSource()->numberOfRows()) {
    return false;
  }
  int previousColumn = selectedColumn();
  int previousRow = selectedRow();
  selectColumn(i);
  selectRow(j);

  /* The delegate is notified:
   * - after changing the selected cell but before scrolling: for instance,
   *   ExpressionModelListController needs to update its memoized cell before
   *   being able to scroll;
   * - after scrolling: for instance, the calculation history table might
   *   change its cell content when selected (output toggling, ellipsis toggling)
   *   and thus need to access the right used cell - which is defined only
   *   after scrolling.
   */

  if (m_delegate) {
    m_delegate->tableViewDidChangeSelection(this, previousColumn, previousRow, withinTemporarySelection);
    if (selectedColumn() == previousColumn && selectedRow() == previousRow) {
      return false;
    }
  }

  // Unhighlight previous cell
  HighlightCell * previousCell = cellAtLocation(previousColumn, previousRow);
  if (previousCell) {
    previousCell->setHighlighted(false); // Must be done before scrolling because if previousCell becomes hidden, setHighlighted will not reload
  }

  // Scroll
  if (selectedRow() >= 0) {
    scrollToCell(selectedColumn(), selectedRow());
  }
  if (m_delegate) {
    m_delegate->tableViewDidChangeSelectionAndDidScroll(this, previousColumn, previousRow, withinTemporarySelection);
  }

  HighlightCell * cell = selectedCell();
  if (cell) {
    // Update first responder
    if ((selectedColumn() != previousColumn || selectedRow() != previousRow) && setFirstResponder) {
      Container::activeApp()->setFirstResponder(cell->responder() ? cell->responder() : this);
    }
    // Highlight new cell
    cell->setHighlighted(true);
  }

  return true;
}

bool SelectableTableView::selectCellAtClippedLocation(int i, int j, bool setFirstResponder, bool withinTemporarySelection) {
  i = std::clamp(i, 0, dataSource()->numberOfColumns() - 1);
  j = std::clamp(j, 0, dataSource()->numberOfRows() - 1);
  if (j == selectedRow() && i == selectedColumn()) {
    // Cell was already selected.
    return false;
  }
  return selectCellAtLocation(i, j, setFirstResponder, withinTemporarySelection);
}

bool SelectableTableView::handleEvent(Ion::Events::Event event) {
  assert(dataSource()->numberOfRows() > 0);
  int step = Ion::Events::longPressFactor();
  if (event == Ion::Events::Down) {
    return selectCellAtClippedLocation(selectedColumn(), indexOfNextSelectableRow(step));
  }
  if (event == Ion::Events::Up) {
    return selectCellAtClippedLocation(selectedColumn(), indexOfNextSelectableRow(-step));
  }
  if (event == Ion::Events::Left) {
    return selectCellAtClippedLocation(indexOfNextSelectableColumn(-step), selectedRow());
  }
  if (event == Ion::Events::Right) {
    return selectCellAtClippedLocation(indexOfNextSelectableColumn(step), selectedRow());
  }
  if (event == Ion::Events::Copy || event == Ion::Events::Cut || event == Ion::Events::Sto || event == Ion::Events::Var) {
    HighlightCell * cell = selectedCell();
    if (cell == nullptr) {
      return false;
    }
    const char * text = cell->text();
    if (text) {
      if (event == Ion::Events::Sto || event == Ion::Events::Var) {
        Container::activeApp()->storeValue(text);
      } else {
        Escher::Clipboard::SharedClipboard()->store(text);
      }
      return true;
    }
    Poincare::Layout l = cell->layout();
    if (!l.isUninitialized()) {
      constexpr int bufferSize = TextField::MaxBufferSize();
      char buffer[bufferSize];
      l.serializeParsedExpression(buffer, bufferSize, m_delegate == nullptr ? nullptr : m_delegate->context());
      if (event == Ion::Events::Sto || event == Ion::Events::Var) {
        Container::activeApp()->storeValue(buffer);
      } else {
        Escher::Clipboard::SharedClipboard()->store(buffer);
      }
      return true;
    }
  }
  return false;
}

void SelectableTableView::unhighlightSelectedCell() {
  if (!selectedCell()) {
    return;
  }
  HighlightCell * cell = cellAtLocation(selectedColumn(), selectedRow());
  /* Previous cell does not always exist.
   * For example, unhighlightSelectedCell can be called twice:
   * - from selectCellAtLocation
   * - and then from m_delegate->tableViewDidChangeSelection inside unhighlightSelectedCell
   * The first call selects an invisible cell. At the time of the second call,
   * the selected cell might be still invisible because scrolling happens
   * after. */
  if (cell) {
    cell->setHighlighted(false);
  }
}

void SelectableTableView::deselectTable(bool withinTemporarySelection) {
  unhighlightSelectedCell();
  int previousSelectedCellX = selectedColumn();
  int previousSelectedCellY = selectedRow();
  selectColumn(0);
  selectRow(-1);
  if (m_delegate) {
    m_delegate->tableViewDidChangeSelection(this, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
    m_delegate->tableViewDidChangeSelectionAndDidScroll(this, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
  }
}

void SelectableTableView::reloadData(bool setFirstResponder, bool setSelection) {
  dataSource()->initCellSize(this);
  int col = selectedColumn();
  int row = selectedRow();
  // TODO: remove setSelection? (Cf comment in DynamicCellsDataSource)
  if (setSelection) {
    deselectTable(true);
  }
  /* FIXME: The problem with calling deselectTable is that at this point in time
   * the datasource's model is very likely to have changed. Therefore it's
   * rather complicated to get a pointer to the currently selected cell (in
   * order to deselect it). */
  /* As a workaround, datasources can reset the highlighted state in their
   * willDisplayCell callback. */
  TableView::layoutSubviews();
  if (setSelection) {
    selectCellAtLocation(col, row, setFirstResponder, true);
  }
}

void SelectableTableView::didEnterResponderChain(Responder * previousFirstResponder) {
  int col = selectedColumn();
  int row = std::max(selectedRow(), firstSelectableRow());
  selectColumn(0);
  selectRow(-1);
  selectCellAtLocation(col, row);
}

void SelectableTableView::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder != nullptr) {
    unhighlightSelectedCell();
  }
}

}