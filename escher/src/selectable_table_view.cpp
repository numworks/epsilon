#include <escher/selectable_table_view.h>
#include <escher/container.h>
#include <escher/clipboard.h>
#include <escher/metric.h>
#include <assert.h>

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

int SelectableTableView::selectedRow() {
  return m_selectionDataSource->selectedRow();
}

int SelectableTableView::selectedColumn() {
  return m_selectionDataSource->selectedColumn();
}

void SelectableTableView::selectRow(int j) {
  m_selectionDataSource->selectRow(j);
}

void SelectableTableView::selectColumn(int i) {
  m_selectionDataSource->selectColumn(i);
}

void SelectableTableView::reloadData(bool setFirstResponder) {
  int col = selectedColumn();
  int row = selectedRow();
  deselectTable(true);
  /* FIXME: The problem with calling deselectTable is that at this point in time
   * the datasource's model is very likely to have changed. Therefore it's
   * rather complicated to get a pointer to the currently selected cell (in
   * order to deselect it). */
  /* As a workaround, datasources can reset the highlighted state in their
   * willDisplayCell callback. */
  TableView::layoutSubviews();
  selectCellAtLocation(col, row, setFirstResponder, true);
}

void SelectableTableView::didEnterResponderChain(Responder * previousFirstResponder) {
  int col = selectedColumn();
  int row = selectedRow();
  selectColumn(0);
  selectRow(-1);
  selectCellAtLocation(col, row);
}

void SelectableTableView::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder != nullptr) {
    unhighlightSelectedCell();
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

bool SelectableTableView::selectCellAtLocation(int i, int j, bool setFirstResponder, bool withinTemporarySelection) {
  if (i < 0 || i >= dataSource()->numberOfColumns()) {
    return false;
  }
  if (j < 0 || j >= dataSource()->numberOfRows()) {
    return false;
  }
  unhighlightSelectedCell();
  int previousX = selectedColumn();
  int previousY = selectedRow();
  selectColumn(i);
  selectRow(j);

  /* The delegate is notified:
   * - after changing the selected cell but before scrolling: for instance,
   *   ExpressionModelListController needs to update its memoized cell before
   *   being able to scroll;
   * - after scrolling: for instance, the calculation history table might
   *   change its cell content when selected (outup toggling, ellipsis toggling)
   *   and thus need to access the right used cell - which is defined only
   *   after scrolling.
   */

  if (m_delegate) {
    m_delegate->tableViewDidChangeSelection(this, previousX, previousY, withinTemporarySelection);
  }

  if (selectedRow() >= 0) {
    scrollToCell(selectedColumn(), selectedRow());
  }

  if (m_delegate) {
    m_delegate->tableViewDidChangeSelectionAndDidScroll(this, previousX, previousY, withinTemporarySelection);
  }

  HighlightCell * cell = selectedCell();
  if (cell) {
    // Update first responder
    if ((i != previousX || j != previousY) && setFirstResponder) {
      Container::activeApp()->setFirstResponder(cell->responder() ? cell->responder() : this);
    }
  }

  cell = selectedCell();
  if (cell) {
    cell->setHighlighted(true);
  }
  return true;
}

bool SelectableTableView::selectCellAtClippedLocation(int i, int j, bool setFirstResponder, bool withinTemporarySelection) {
  if (i < 0) {
    i = 0;
  } else if (i >= dataSource()->numberOfColumns()) {
    i = dataSource()->numberOfColumns() - 1;
  }
  if (j < 0) {
    j = 0;
  } else if (j >= dataSource()->numberOfRows()) {
    j = dataSource()->numberOfRows() - 1;
  }
  if (j == selectedRow() && i == selectedColumn()) {
    // Cell was already selected.
    return false;
  }
  return selectCellAtLocation(i, j, setFirstResponder, withinTemporarySelection);
}

HighlightCell * SelectableTableView::selectedCell() {
  if (selectedColumn() < 0 || selectedRow() < 0) {
    return nullptr;
  }
  return cellAtLocation(selectedColumn(), selectedRow());
}

bool SelectableTableView::handleEvent(Ion::Events::Event event) {
  int step = Ion::Events::repetitionFactor();
  if (event == Ion::Events::Down) {
    return selectCellAtClippedLocation(selectedColumn(), selectedRow() + step);
  }
  if ((event == Ion::Events::ShiftDown || event == Ion::Events::AlphaDown) && selectedRow() < dataSource()->numberOfRows()-1) {
    return selectCellAtLocation(selectedColumn(), dataSource()->numberOfRows()-1);
  }
  if (event == Ion::Events::Up) {
    return selectCellAtClippedLocation(selectedColumn(), selectedRow() - step);
  }
  if ((event == Ion::Events::ShiftUp || event == Ion::Events::AlphaUp) && selectedRow() > 0) {
    return selectCellAtLocation(selectedColumn(), 0);
  }
  if (event == Ion::Events::Left) {
    return selectCellAtClippedLocation(selectedColumn() - step, selectedRow());
  }
  if ((event == Ion::Events::ShiftLeft || event == Ion::Events::AlphaLeft) && selectedColumn() > 0) {
    return selectCellAtLocation(0, selectedRow());
  }
  if (event == Ion::Events::Right) {
    return selectCellAtClippedLocation(selectedColumn() + step, selectedRow());
  }
  if ((event == Ion::Events::ShiftRight || event == Ion::Events::AlphaRight) && selectedColumn() < dataSource()->numberOfColumns()-1) {
    return selectCellAtLocation(dataSource()->numberOfColumns()-1, selectedRow());
  }
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    HighlightCell * cell = selectedCell();
    if (cell == nullptr) {
      return false;
    }
    const char * text = cell->text();
    if (text) {
      Clipboard::sharedClipboard()->store(text);
      return true;
    }
    Poincare::Layout l = cell->layout();
    if (!l.isUninitialized()) {
      constexpr int bufferSize = TextField::maxBufferSize();
      char buffer[bufferSize];
      l.serializeParsedExpression(buffer, bufferSize, m_delegate == nullptr ? nullptr : m_delegate->context());
      Clipboard::sharedClipboard()->store(buffer);
      return true;
    }
  }
  return false;
}

void SelectableTableView::unhighlightSelectedCell() {
  if (selectedColumn() >= 0 && selectedColumn() < dataSource()->numberOfColumns() &&
      selectedRow() >= 0 && selectedRow() < dataSource()->numberOfRows()) {
    HighlightCell * previousCell = cellAtLocation(selectedColumn(), selectedRow());
    /* Previous cell does not always exist.
     * For example, unhighlightSelectedCell can be called twice:
     * - from selectCellAtLocation
     * - and then from m_delegate->tableViewDidChangeSelection inside unhighlightSelectedCell
     * The first call selects an invisible cell. At the time of the second call,
     * the selected cell might be still invisible because scrolling happens
     * after. */
    if (previousCell) {
      previousCell->setHighlighted(false);
    }
  }
}
