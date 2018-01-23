#include <escher/selectable_table_view.h>
#include <assert.h>

SelectableTableView::SelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource, KDCoordinate horizontalCellOverlapping, KDCoordinate verticalCellOverlapping, KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin,
    SelectableTableViewDataSource * selectionDataSource, SelectableTableViewDelegate * delegate, bool showIndicators, bool colorBackground, KDColor backgroundColor,
    KDCoordinate indicatorThickness, KDColor indicatorColor, KDColor backgroundIndicatorColor, KDCoordinate indicatorMargin) :
  TableView(dataSource, selectionDataSource, horizontalCellOverlapping, verticalCellOverlapping, topMargin, rightMargin, bottomMargin, leftMargin, showIndicators, colorBackground, backgroundColor,
    indicatorThickness, indicatorColor, backgroundIndicatorColor, indicatorMargin),
  Responder(parentResponder),
  m_selectionDataSource(selectionDataSource),
  m_delegate(delegate)
{
  assert(m_selectionDataSource != nullptr);
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
  deselectTable();
  /* FIXME: The problem with calling deselectTable is that at this point in time
   * the datasource's model is very likely to have changed. Therefore it's
   * rather complicated to get a pointer to the currently selected cell (in
   * order to deselect it). */
  /* As a workaround, datasources can reset the highlighted state in their
   * willDisplayCell callback. */
  TableView::layoutSubviews();
  selectCellAtLocation(col, row, setFirstResponder);
}

void SelectableTableView::didEnterResponderChain(Responder * previousFirstResponder) {
  int col = selectedColumn();
  int row = selectedRow();
  selectColumn(0);
  selectRow(-1);
  selectCellAtLocation(col, row);
}

void SelectableTableView::willExitResponderChain(Responder * nextFirstResponder) {
  unhighlightSelectedCell();
}

void SelectableTableView::deselectTable() {
  unhighlightSelectedCell();
  int previousSelectedCellX = selectedColumn();
  int previousSelectedCellY = selectedRow();
  selectColumn(0);
  selectRow(-1);
  if (m_delegate) {
    m_delegate->tableViewDidChangeSelection(this, previousSelectedCellX, previousSelectedCellY);
  }
}

bool SelectableTableView::selectCellAtLocation(int i, int j, bool setFirstResponder) {
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
  if (selectedRow() >= 0) {
    scrollToCell(i, j);
  }
  HighlightCell * cell = selectedCell();
  if (cell) {
    cell->setHighlighted(true);
    // Update first responder
    if ((i != previousX || j != previousY) && setFirstResponder) {
      if (cell->responder()) {
        app()->setFirstResponder(cell->responder());
      } else {
        app()->setFirstResponder(this);
      }
    }
  }
  if (m_delegate) {
    m_delegate->tableViewDidChangeSelection(this, previousX, previousY);
  }
  return true;
}

HighlightCell * SelectableTableView::selectedCell() {
  if (selectedColumn() < 0 || selectedRow() < 0) {
    return nullptr;
  }
  return cellAtLocation(selectedColumn(), selectedRow());
}

bool SelectableTableView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    return selectCellAtLocation(selectedColumn(), selectedRow()+1);
  }
  if (event == Ion::Events::Up) {
    return selectCellAtLocation(selectedColumn(), selectedRow()-1);
  }
  if (event == Ion::Events::Left) {
    return selectCellAtLocation(selectedColumn()-1, selectedRow());
  }
  if (event == Ion::Events::Right) {
    return selectCellAtLocation(selectedColumn()+1, selectedRow());
  }
  return false;
}

void SelectableTableView::unhighlightSelectedCell() {
  if (selectedColumn() >= 0 && selectedColumn() < dataSource()->numberOfColumns() &&
      selectedRow() >= 0 && selectedRow() < dataSource()->numberOfRows()) {
    HighlightCell * previousCell = cellAtLocation(selectedColumn(), selectedRow());
    previousCell->setHighlighted(false);
  }
}
