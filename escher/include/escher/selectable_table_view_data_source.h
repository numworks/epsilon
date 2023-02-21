#ifndef ESCHER_SELECTABLE_TABLE_VIEW_DATA_SOURCE_H
#define ESCHER_SELECTABLE_TABLE_VIEW_DATA_SOURCE_H

#include <escher/scroll_view_data_source.h>

namespace Escher {

class SelectableTableViewDataSource : public ScrollViewDataSource {
 public:
  SelectableTableViewDataSource() : m_selectedColumn(0), m_selectedRow(-1) {}
  int selectedColumn() const { return m_selectedColumn; }
  int selectedRow() const { return m_selectedRow; }
  void selectColumn(int i) { m_selectedColumn = i; }
  void selectRow(int j) { m_selectedRow = j; }
  void selectCellAtLocation(int i, int j) {
    m_selectedColumn = i;
    m_selectedRow = j;
  }

 private:
  int m_selectedColumn;
  int m_selectedRow;
};

}  // namespace Escher
#endif
