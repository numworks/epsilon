#include <escher/selectable_table_view.h>
#include <assert.h>

SelectableTableView::SelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource, KDCoordinate horizontalCellOverlapping, KDCoordinate verticalCellOverlapping, KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin,
    SelectableTableViewDelegate * delegate, bool showIndicators, bool colorBackground, KDColor backgroundColor,
    KDCoordinate indicatorThickness, KDColor indicatorColor, KDColor backgroundIndicatorColor, KDCoordinate indicatorMargin) :
  TableView(dataSource, horizontalCellOverlapping, verticalCellOverlapping, topMargin, rightMargin, bottomMargin, leftMargin, showIndicators, colorBackground, backgroundColor,
    indicatorThickness, indicatorColor, backgroundIndicatorColor, indicatorMargin),
  Responder(parentResponder),
  m_delegate(delegate)
{
  assert(m_delegate != nullptr);
}

int SelectableTableView::selectedRow() {
  return m_delegate->selectedRow();
}

int SelectableTableView::selectedColumn() {
  return m_delegate->selectedColumn();
}

void SelectableTableView::selectRow(int j) {
  m_delegate->selectRow(j);
}

void SelectableTableView::selectColumn(int i) {
  m_delegate->selectColumn(i);
}

void SelectableTableView::didBecomeFirstResponder() {
  selectCellAtLocation(selectedColumn(), selectedRow());
  m_delegate->tableViewDidChangeSelection(this, 0, -1);
}

void SelectableTableView::willExitResponderChain(Responder * nextFirstResponder) {
  unhighlightTable();
}

void SelectableTableView::deselectTable() {
  unhighlightTable();
  int previousSelectedCellX = selectedColumn();
  int previousSelectedCellY = selectedRow();
  selectColumn(0);
  selectRow(-1);
  if (m_delegate) {
    m_delegate->tableViewDidChangeSelection(this, previousSelectedCellX, previousSelectedCellY);
  }
}

bool SelectableTableView::selectCellAtLocation(int i, int j) {
  if (i < 0 || i >= dataSource()->numberOfColumns()) {
    return false;
  }
  if (j < 0 || j >= dataSource()->numberOfRows()) {
    return false;
  }
  unhighlightTable();
  int previousX = selectedColumn();
  int previousY = selectedRow();
  selectColumn(i);
  selectRow(j);
  if (selectedRow() >= 0) {
    scrollToCell(i, j);
    HighlightCell * cell = cellAtLocation(i, j);
    cell->setHighlighted(true);
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

void SelectableTableView::unhighlightTable() {
  if (selectedColumn() >= 0 && selectedColumn() < dataSource()->numberOfColumns() &&
      selectedRow() >= 0 && selectedRow() < dataSource()->numberOfRows()) {
    HighlightCell * previousCell = cellAtLocation(selectedColumn(), selectedRow());
    previousCell->setHighlighted(false);
  }
}
