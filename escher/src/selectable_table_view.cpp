#include <assert.h>
#include <escher/clipboard.h>
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/selectable_table_view.h>
#include <string.h>

namespace Escher {

SelectableTableView::SelectableTableView(
    Responder* parentResponder, TableViewDataSource* dataSource,
    SelectableTableViewDataSource* selectionDataSource,
    SelectableTableViewDelegate* delegate,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : TableView(dataSource, selectionDataSource, scrollViewDelegate),
      Responder(parentResponder),
      m_selectionDataSource(selectionDataSource),
      m_delegate(delegate) {
  assert(m_selectionDataSource != nullptr);
  setMargins(Metric::CommonMargins);
}

HighlightCell* SelectableTableView::selectedCell() {
  return cellAtLocation(selectedColumn(), selectedRow());
}

int SelectableTableView::maxIndexInDirection(int col, int row,
                                             OMG::Direction direction) {
  assert(numberOfRowsAtColumn(col) <= totalNumberOfRows());
  int numberOfIndexes = direction.isVertical() ? numberOfRowsAtColumn(col)
                                               : totalNumberOfColumns();
  return numberOfIndexes - 1;
}

int SelectableTableView::lastSelectableIndexInDirection(
    int col, int row, OMG::Direction direction) {
  bool searchForRow = direction.isVertical();
  const int maxIndex = maxIndexInDirection(col, row, direction);
  if (maxIndex < 0) {
    return 0;
  }
  int firstIndex, lastIndex, step;
  if (direction.isUp() || direction.isLeft()) {
    firstIndex = 0;
    lastIndex = maxIndex;
    step = 1;
  } else {
    assert(direction.isDown() || direction.isRight());
    firstIndex = maxIndex;
    lastIndex = 0;
    step = -1;
  }
  for (int index = firstIndex; step * index <= step * lastIndex;
       index += step) {
    bool isSelectable = searchForRow ? canSelectCellAtLocation(col, index)
                                     : canSelectCellAtLocation(index, row);
    if (isSelectable) {
      return index;
    }
  }
  assert(false);
  return -1;
}

bool SelectableTableView::canSelectCellAtLocation(int column, int row) {
  assert(0 <= column && column < totalNumberOfColumns());
  assert(0 <= row && row < totalNumberOfRows());
  const HighlightCell* cell = cellAtLocation(column, row);
  return (!cell || cell->isVisible()) &&
         dataSource()->canSelectCellAtLocation(column, row);
}

void SelectableTableView::nextSelectableCellInDirection(
    int* col, int* row, OMG::Direction direction, int delta) {
  bool searchForRow = direction.isVertical();
  assert((searchForRow && *col < totalNumberOfColumns() && *col >= 0) ||
         (!searchForRow && *row < numberOfRowsAtColumn(*col) && *row >= 0));
  assert(numberOfRowsAtColumn(*col) <= totalNumberOfRows());
  assert(delta > 0);

  int index = searchForRow ? *row : *col;
  int selectableIndex = -1;
  int step = direction.isDown() || direction.isRight() ? 1 : -1;
  const int maxIndex = maxIndexInDirection(*col, *row, direction);
  while (delta) {
    index += step;
    if (index < 0 || index > maxIndex) {
      if (selectableIndex < 0) {
        selectableIndex = lastSelectableIndexInDirection(*col, *row, direction);
      }
      break;
    }
    bool cellIsSelectable = searchForRow ? canSelectCellAtLocation(*col, index)
                                         : canSelectCellAtLocation(index, *row);
    if (cellIsSelectable) {
      selectableIndex = index;
      delta--;
    } else if (!searchForRow) {
      int newRow = std::clamp(*row, 0, numberOfRowsAtColumn(index) - 1);
      if (canSelectCellAtLocation(index, newRow)) {
        *col = index;
        *row = newRow;
        return;
      }
    }
  }
  (searchForRow ? *row : *col) = selectableIndex;
}

void SelectableTableView::selectCellAtLocation(int col, int row,
                                               bool setFirstResponder,
                                               bool withinTemporarySelection) {
  // Table deselected
  if (row == -1) {
    return;
  }

  // Clip col and row
  int nCols = totalNumberOfColumns();
  if (nCols == 0) {
    return;
  }
  col = std::clamp(col, 0, nCols - 1);
  int nRows = numberOfRowsAtColumn(col);
  assert(nRows <= totalNumberOfRows());
  if (nRows == 0) {
    return;
  }
  row = std::clamp(row, 0, nRows - 1);

  if (!canSelectCellAtLocation(col, row)) {
    /* If the cell is not selectable, go down by default.
     * This behaviour is only implemented for Explicit. */
    nextSelectableCellInDirection(&col, &row, OMG::Direction::Down());
  }
  // There should always be at least 1 selectable cell in the column
  assert(canSelectCellAtLocation(col, row));

  // Unhighlight previous cell
  unhighlightSelectedCell();

  int previousColumn = selectedColumn();
  int previousRow = selectedRow();
  KDPoint previousOffset = contentOffset();

  // Selection
  selectColumn(col);
  selectRow(row);
  assert(selectedColumn() == col && selectedRow() == row);

  // Scroll
  scrollToCell(col, row);

  if (m_delegate) {
    m_delegate->tableViewDidChangeSelectionAndDidScroll(
        this, previousColumn, previousRow, previousOffset,
        withinTemporarySelection);
  }

  HighlightCell* cell = selectedCell();
  if (cell) {
    // Update first responder
    Responder* r = cell->responder() ? cell->responder() : this;
    if (setFirstResponder &&
        /* Sometimes reusable cells must be re-set as first responder if the row
           changed. Other times, the row did not change but the responder did
           (when going back in previous menu for example). */
        ((col != previousColumn || row != previousRow) ||
         App::app()->firstResponder() != r)) {
      App::app()->setFirstResponder(r, true);
    }
    // Highlight new cell
    cell->setHighlighted(true);
  }
}

bool SelectableTableView::handleEvent(Ion::Events::Event event) {
  assert(totalNumberOfRows() > 0);
  int delta = Ion::Events::longPressFactor();
  int col = selectedColumn();
  int row = selectedRow();
  if (event.isMoveEvent()) {
    OMG::Direction direction = event.direction();
    nextSelectableCellInDirection(&col, &row, direction, delta);
    if (col == selectedColumn() && row == selectedRow()) {
      // Cell was already selected.
      return false;
    }
    selectCellAtLocation(col, row);
    return true;
  }
  if (event == Ion::Events::Copy || event == Ion::Events::Cut ||
      event == Ion::Events::Sto || event == Ion::Events::Var) {
    HighlightCell* cell = selectedCell();
    if (!cell) {
      return false;
    }
    const char* text = cell->text();
    Poincare::Layout layout = cell->layout();
    if (!text && layout.isUninitialized()) {
      return false;
    }
    if (!App::app()->canStoreLayout()) {
      /* Apps without a store menu cannot handle layouts, but they can copy or
       * cut text */
      assert(text);
      if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
        Escher::Clipboard::SharedClipboard()->storeText(text);
      }
      return true;
    }
    Poincare::Layout toStore;
    // Step 1: Determine text to store
    if (dataSource()->canStoreCellAtLocation(selectedColumn(), selectedRow())) {
      if (text) {
        toStore = Poincare::Layout::String(text);
      } else {
        assert(!layout.isUninitialized());
        toStore = layout;
      };
    }
    // Step 2: Determine where to store it
    if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
      if (!toStore.isUninitialized()) {
        // We don't want to store an empty text in clipboard
        Escher::Clipboard::SharedClipboard()->storeLayout(toStore);
      }
    } else {
      assert(event == Ion::Events::Sto || event == Ion::Events::Var);
      App::app()->storeLayout(toStore);
    }
    return true;
  }
  return false;
}

void SelectableTableView::unhighlightSelectedCell() {
  HighlightCell* cell = selectedCell();
  if (cell) {
    cell->setHighlighted(false);
  }
}

void SelectableTableView::deselectTable(bool withinTemporarySelection) {
  unhighlightSelectedCell();
  int previousSelectedCol = selectedColumn();
  int previousSelectedRow = selectedRow();
  KDPoint previousOffset = contentOffset();
  selectRow(-1);
  if (m_delegate) {
    m_delegate->tableViewDidChangeSelectionAndDidScroll(
        this, previousSelectedCol, previousSelectedRow, previousOffset,
        withinTemporarySelection);
  }
}

void SelectableTableView::reloadData(bool setFirstResponder,
                                     bool resetMemoization) {
  if (resetMemoization) {
    resetSizeAndOffsetMemoization();
  }
  int col = selectedColumn();
  int row = selectedRow();
  KDPoint offset = offsetToRestoreAfterReload();
  deselectTable(true);
  layoutSubviews();
  setClippedContentOffset(offset);
  selectCellAtLocation(col, row, setFirstResponder, true);
}

void SelectableTableView::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  switch (event.type) {
    case ResponderChainEventType::HasBecomeFirst: {
      HighlightCell* cell = selectedCell();
      if (cell && cell->responder()) {
        // Update first responder
        App::app()->setFirstResponder(cell->responder());
      }
      break;
    }
    case ResponderChainEventType::DidEnter: {
      int col = selectedColumn();
      int row = selectedRow();
      selectRow(-1);
      selectCellAtLocation(col, row, false);
      break;
    }
    case ResponderChainEventType::WillExit: {
      if (event.nextFirstResponder != nullptr) {
        unhighlightSelectedCell();
      }
    }
    default: {
      Responder::handleResponderChainEvent(event);
      break;
    }
  }
}

void SelectableTableView::layoutSubviews(bool force) {
  TableView::layoutSubviews(force);
  /* The highlighting/unhighlighting of cells might have failed when
   * the table was reloaded because the reusable cells were not populated.
   * To prevent this, their Highlight status is reloaded here. */
  int nReusableCells = numberOfDisplayableCells();
  HighlightCell* thisSelectedCell = selectedCell();
  for (int cellIndex = 0; cellIndex < nReusableCells; cellIndex++) {
    HighlightCell* cell = reusableCellAtIndex(cellIndex);
    if (cell && cell->isVisible()) {
      cell->setHighlighted(thisSelectedCell == cell);
    }
  }
}

}  // namespace Escher
