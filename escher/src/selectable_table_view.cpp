#include <escher/selectable_table_view.h>

SelectableTableView::SelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource,
    KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin,
    SelectableTableViewDelegate * delegate, bool showIndicators, bool colorBackground,
    KDColor backgroundColor) :
  TableView(dataSource, topMargin, rightMargin, bottomMargin, leftMargin, showIndicators, colorBackground, backgroundColor),
  Responder(parentResponder),
  m_delegate(delegate),
  m_selectedCellX(0),
  m_selectedCellY(-1)
{
}

int SelectableTableView::selectedRow() {
  return m_selectedCellY;
}

int SelectableTableView::selectedColumn() {
  return m_selectedCellX;
}

void SelectableTableView::didBecomeFirstResponder() {
  reloadData();
  if (m_delegate) {
    m_delegate->tableViewDidChangeSelection(this, 0, -1);
  }
}

void SelectableTableView::deselectTable() {
  if (m_selectedCellX >= 0 && m_selectedCellX < dataSource()->numberOfColumns() &&
      m_selectedCellY >= 0 && m_selectedCellY < dataSource()->numberOfRows()) {
    TableViewCell * previousCell = cellAtLocation(m_selectedCellX, m_selectedCellY);
    previousCell->setHighlighted(false);
  }
  m_selectedCellX = 0;
  m_selectedCellY = -1;
}

bool SelectableTableView::selectCellAtLocation(int i, int j) {
  if (i < 0 || i >= dataSource()->numberOfColumns()) {
    return false;
  }
  if (j < 0 || j >= dataSource()->numberOfRows()) {
    return false;
  }

  if (m_selectedCellX >= 0 && m_selectedCellX < dataSource()->numberOfColumns() &&
      m_selectedCellY >= 0 && m_selectedCellY < dataSource()->numberOfRows()) {
    TableViewCell * previousCell = cellAtLocation(m_selectedCellX, m_selectedCellY);
    previousCell->setHighlighted(false);
  }
  m_selectedCellX = i;
  m_selectedCellY = j;
  if (m_selectedCellY >= 0) {
    scrollToCell(i, j);
    TableViewCell * cell = cellAtLocation(i, j);
    cell->setHighlighted(true);
  }
  return true;
}

TableViewCell * SelectableTableView::selectedCell() {
  return cellAtLocation(m_selectedCellX, m_selectedCellY);
}

bool SelectableTableView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down && selectCellAtLocation(m_selectedCellX, m_selectedCellY+1)) {
    if (m_delegate) {
      m_delegate->tableViewDidChangeSelection(this, m_selectedCellX, m_selectedCellY - 1);
    }
    return true;
  }
  if (event == Ion::Events::Up && selectCellAtLocation(m_selectedCellX, m_selectedCellY-1)) {
    if (m_delegate) {
      m_delegate->tableViewDidChangeSelection(this, m_selectedCellX, m_selectedCellY + 1);
    }
    return true;
  }
  if (event == Ion::Events::Left && selectCellAtLocation(m_selectedCellX-1, m_selectedCellY)) {
    if (m_delegate) {
      m_delegate->tableViewDidChangeSelection(this, m_selectedCellX + 1, m_selectedCellY);
    }
    return true;
  }
  if (event == Ion::Events::Right && selectCellAtLocation(m_selectedCellX+1, m_selectedCellY)) {
    if (m_delegate) {
      m_delegate->tableViewDidChangeSelection(this, m_selectedCellX - 1, m_selectedCellY);
    }
    return true;
  }
  return false;
}
