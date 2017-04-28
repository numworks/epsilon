#include <escher/selectable_table_view_delegate.h>

SelectableTableViewDelegate::SelectableTableViewDelegate() :
  m_selectedCellX(0),
  m_selectedCellY(-1)
{
}

int SelectableTableViewDelegate::selectedRow() {
  return m_selectedCellY;
}

int SelectableTableViewDelegate::selectedColumn() {
  return m_selectedCellX;
}

void SelectableTableViewDelegate::selectRow(int j) {
  m_selectedCellY = j;
}

void SelectableTableViewDelegate::selectColumn(int i) {
  m_selectedCellX = i;
}

void SelectableTableViewDelegate::selectCellAtLocation(int i, int j) {
  m_selectedCellX = i;
  m_selectedCellY = j;
}

void SelectableTableViewDelegate::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
}

