#include <escher/selectable_table_view_data_source.h>

SelectableTableViewDataSource::SelectableTableViewDataSource() :
  m_selectedCellX(0),
  m_selectedCellY(-1)
{
}

int SelectableTableViewDataSource::selectedRow() {
  return m_selectedCellY;
}

int SelectableTableViewDataSource::selectedColumn() {
  return m_selectedCellX;
}

void SelectableTableViewDataSource::selectRow(int j) {
  m_selectedCellY = j;
}

void SelectableTableViewDataSource::selectColumn(int i) {
  m_selectedCellX = i;
}

void SelectableTableViewDataSource::selectCellAtLocation(int i, int j) {
  m_selectedCellX = i;
  m_selectedCellY = j;
}
