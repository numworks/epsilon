#include <escher/selectable_table_view.h>
#include <escher/container.h>
#include <escher/clipboard.h>
#include <escher/metric.h>
#include <assert.h>
#include <string.h>

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
  return cellAtLocation(selectedColumn(), selectedRow());
}

int SelectableTableView::firstOrLastSelectableColumnOrRow(bool first, bool searchForRow) {
  int nColumnsOrRow = searchForRow ? dataSource()->numberOfRows() : dataSource()->numberOfColumns();
  if (nColumnsOrRow == 0) {
    return 0;
  }
  int firstIndex = first ? 0 : nColumnsOrRow - 1;
  for (int cow = firstIndex; first ? cow < nColumnsOrRow : cow >= 0; first ? cow++ : cow--) {
    bool isSelectable = searchForRow ? cellAtLocationIsSelectable(selectedColumn(), cow) : cellAtLocationIsSelectable(cow, selectedRow());
    if (isSelectable) {
      return cow;
    }
  }
  assert(false);
  return -1;
}

int SelectableTableView::indexOfNextSelectableColumnOrRow(int delta, int col, int row, bool searchForRow) {
  assert((searchForRow && col < dataSource()->numberOfColumns() && col >= 0)
          || (!searchForRow && row < dataSource()->numberOfRows() && row >= 0));
  assert(delta != 0);
  // Let's call our variable cow, as a shortcut for col-or-row
  int cow = searchForRow ? row : col;
  int selectableCow = -1;
  int step = delta > 0 ? 1 : -1;
  const int lastCow = (searchForRow ? dataSource()->numberOfRows() : dataSource()->numberOfColumns()) - 1;
  while (delta) {
    cow += step;
    if (cow < 0 || cow > lastCow) {
      if (selectableCow >= 0) {
        return selectableCow;
      }
      return firstOrLastSelectableColumnOrRow(delta < 0, searchForRow);
    }
    bool cellIsSelectable = searchForRow ? cellAtLocationIsSelectable(col, cow) : cellAtLocationIsSelectable(cow, row);
    if (cellIsSelectable) {
      selectableCow = cow;
      delta -= step;
    }
  }
  return selectableCow;
}

bool SelectableTableView::selectCellAtLocation(int col, int row, bool setFirstResponder, bool withinTemporarySelection) {
  if (row < 0 || col < 0 || row >= dataSource()->numberOfRows() || col >= dataSource()->numberOfColumns()) {
    return false;
  }

  if (!cellAtLocationIsSelectable(col, row)) {
    /* If the cell is not selectable, go down by default.
     * This behaviour is only implemented for Explicit
     */
    row = indexOfNextSelectableRow(1, col, row);
  }
  // There should always be at least 1 selectable cell in the column
  assert(cellAtLocationIsSelectable(col, row));

  int previousColumn = selectedColumn();
  int previousRow = selectedRow();
  selectColumn(col);
  selectRow(row);

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

bool SelectableTableView::selectCellAtClippedLocation(int col, int row, bool setFirstResponder, bool withinTemporarySelection) {
  col = std::clamp(col, 0, dataSource()->numberOfColumns() - 1);
  row = std::clamp(row, 0, dataSource()->numberOfRows() - 1);
  if (row == selectedRow() && col == selectedColumn()) {
    // Cell was already selected.
    return false;
  }
  return selectCellAtLocation(col, row, setFirstResponder, withinTemporarySelection);
}

bool SelectableTableView::handleEvent(Ion::Events::Event event) {
  assert(dataSource()->numberOfRows() > 0);
  int step = Ion::Events::longPressFactor();
  int col = selectedColumn();
  int row = selectedRow();
  if (event == Ion::Events::Down) {
    return selectCellAtClippedLocation(col, indexOfNextSelectableRow(step, col, row));
  }
  if (event == Ion::Events::Up) {
    return selectCellAtClippedLocation(col, indexOfNextSelectableRow(-step, col, row));
  }
  if (event == Ion::Events::Left) {
    return selectCellAtClippedLocation(indexOfNextSelectableColumn(-step, col, row), row);
  }
  if (event == Ion::Events::Right) {
    return selectCellAtClippedLocation(indexOfNextSelectableColumn(step, col, row), row);
  }
  if (event == Ion::Events::Copy || event == Ion::Events::Cut || event == Ion::Events::Sto || event == Ion::Events::Var) {
    HighlightCell * cell = selectedCell();
    if (cell && (cell->text() || !cell->layout().isUninitialized())) {
      handleStoreEvent(event == Ion::Events::Copy || event == Ion::Events::Cut);
      return true;
    }
  }
  return false;
}

void SelectableTableView::handleStoreEvent(bool inClipboard) {
  HighlightCell * cell = selectedCell();
  assert(cell);
  assert(cell->text() || !cell->layout().isUninitialized());
  constexpr int bufferSize = TextField::MaxBufferSize();
  char buffer[bufferSize];

  // Step 1: Determine text to store
  if (m_delegate && !m_delegate->canStoreContentOfCellAtLocation(this, selectedColumn(), selectedRow())) {
    if (inClipboard) {
      // We don't want to store an empty text in clipboard
      return;
    }
    strlcpy(buffer, "", bufferSize);
  } else {
    const char * text = cell->text();
    Poincare::Layout l = cell->layout();
    if (text) {
      strlcpy(buffer, text, bufferSize);
    } else {
      assert(!l.isUninitialized());
      l.serializeParsedExpression(buffer, bufferSize, m_delegate == nullptr ? nullptr : m_delegate->context());
    }
  }

  // Step 2: Determine where to store it
  if (inClipboard) {
    Escher::Clipboard::SharedClipboard()->store(buffer);
  } else {
    Container::activeApp()->storeValue(buffer);
  }
}

void SelectableTableView::unhighlightSelectedCell() {
  HighlightCell * cell = selectedCell();
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

void SelectableTableView::didBecomeFirstResponder() {
  HighlightCell * cell = selectedCell();
  if (cell && cell->responder()) {
    // Update first responder
    Container::activeApp()->setFirstResponder(cell->responder());
  }
}

void SelectableTableView::didEnterResponderChain(Responder * previousFirstResponder) {
  int col = selectedColumn();
  int row = selectedRow();
  selectColumn(0);
  selectRow(-1);
  selectCellAtLocation(col, row, false);
}

void SelectableTableView::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder != nullptr) {
    unhighlightSelectedCell();
  }
}

}
