#ifndef ESCHER_SELECTABLE_TABLE_VIEW_DATA_SOURCE_H
#define ESCHER_SELECTABLE_TABLE_VIEW_DATA_SOURCE_H

#include <escher/scroll_view_data_source.h>

class SelectableTableView;

class SelectableTableViewDataSource : public ScrollViewDataSource {
public:
  SelectableTableViewDataSource();
  int selectedRow();
  int selectedColumn();
  void selectRow(int j);
  void selectColumn(int i);
  void selectCellAtLocation(int i, int j);
private:
  int m_selectedCellX;
  int m_selectedCellY;
};

#endif
