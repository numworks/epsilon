#ifndef ESCHER_SELECTABLE_TABLE_VIEW_DATA_SOURCE_H
#define ESCHER_SELECTABLE_TABLE_VIEW_DATA_SOURCE_H

#include <escher/scroll_view_data_source.h>

namespace Escher {

class SelectableTableViewDataSource : public ScrollViewDataSource {
 public:
  SelectableTableViewDataSource() : m_selectedColumn(0), m_selectedRow(-1) {}
  int selectedColumn() const { return m_selectedColumn; }
  int selectedRow() const { return m_selectedRow; }
  void selectColumn(int col) { m_selectedColumn = col; }
  void selectRow(int row) { m_selectedRow = row; }
  void selectCellAtLocation(int col, int row) {
    m_selectedColumn = col;
    m_selectedRow = row;
  }

 private:
  int m_selectedColumn;
  int m_selectedRow;
};

}  // namespace Escher
#endif
