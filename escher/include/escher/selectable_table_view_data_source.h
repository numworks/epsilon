#ifndef ESCHER_SELECTABLE_TABLE_VIEW_DATA_SOURCE_H
#define ESCHER_SELECTABLE_TABLE_VIEW_DATA_SOURCE_H

#include <escher/scroll_view_data_source.h>

class SelectableTableViewDataSource : public ScrollViewDataSource {
public:
  SelectableTableViewDataSource() :
    m_selectedCellX(0),
    m_selectedCellY(-1)
  {
  }
  int selectedRow() const { return m_selectedCellY; }
  int selectedColumn() const { return m_selectedCellX; }
  void selectRow(int j) { m_selectedCellY = j; }
  void selectColumn(int i) { m_selectedCellX = i; }
  void selectCellAtLocation(int i, int j) {
    m_selectedCellX = i;
    m_selectedCellY = j;
  }
private:
  int m_selectedCellX;
  int m_selectedCellY;
};

#endif
