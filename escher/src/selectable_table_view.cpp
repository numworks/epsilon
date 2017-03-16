#include <escher/selectable_table_view.h>

SelectableTableView::SelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource, KDCoordinate horizontalCellOverlapping, KDCoordinate verticalCellOverlapping, KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin,
    SelectableTableViewDelegate * delegate, bool showIndicators, bool colorBackground, KDColor backgroundColor,
    KDCoordinate indicatorThickness, KDColor indicatorColor, KDColor backgroundIndicatorColor, KDCoordinate indicatorMargin) :
  TableView(dataSource, horizontalCellOverlapping, verticalCellOverlapping, topMargin, rightMargin, bottomMargin, leftMargin, showIndicators, colorBackground, backgroundColor,
    indicatorThickness, indicatorColor, backgroundIndicatorColor, indicatorMargin),
  Responder(parentResponder),
  m_delegate(delegate),
  m_selectedCellX(0),
  m_selectedCellY(-1)
{
}

void SelectableTableView::setDelegate(SelectableTableViewDelegate * delegate) {
  m_delegate = delegate;
}

int SelectableTableView::selectedRow() {
  return m_selectedCellY;
}

int SelectableTableView::selectedColumn() {
  return m_selectedCellX;
}

void SelectableTableView::didBecomeFirstResponder() {
  if (m_delegate) {
    m_delegate->tableViewDidChangeSelection(this, 0, -1);
  }
}

void SelectableTableView::deselectTable() {
  if (m_selectedCellX >= 0 && m_selectedCellX < dataSource()->numberOfColumns() &&
      m_selectedCellY >= 0 && m_selectedCellY < dataSource()->numberOfRows()) {
    HighlightCell * previousCell = cellAtLocation(m_selectedCellX, m_selectedCellY);
    previousCell->setHighlighted(false);
  }
  int previousSelectedCellX = m_selectedCellX;
  int previousSelectedCellY = m_selectedCellY;
  m_selectedCellX = 0;
  m_selectedCellY = -1;
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
  if (m_selectedCellX >= 0 && m_selectedCellX < dataSource()->numberOfColumns() &&
      m_selectedCellY >= 0 && m_selectedCellY < dataSource()->numberOfRows()) {
    HighlightCell * previousCell = cellAtLocation(m_selectedCellX, m_selectedCellY);
    previousCell->setHighlighted(false);
  }
  int previousX = m_selectedCellX;
  int previousY = m_selectedCellY;
  m_selectedCellX = i;
  m_selectedCellY = j;
  if (m_selectedCellY >= 0) {
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
  return cellAtLocation(m_selectedCellX, m_selectedCellY);
}

bool SelectableTableView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    return selectCellAtLocation(m_selectedCellX, m_selectedCellY+1);
  }
  if (event == Ion::Events::Up) {
    return selectCellAtLocation(m_selectedCellX, m_selectedCellY-1);
  }
  if (event == Ion::Events::Left) {
    return selectCellAtLocation(m_selectedCellX-1, m_selectedCellY);
  }
  if (event == Ion::Events::Right) {
    return selectCellAtLocation(m_selectedCellX+1, m_selectedCellY);
  }
  return false;
}
