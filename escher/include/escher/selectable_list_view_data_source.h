#ifndef ESCHER_SELECTABLE_LIST_VIEW_DATA_SOURCE_H
#define ESCHER_SELECTABLE_LIST_VIEW_DATA_SOURCE_H

#include <escher/selectable_table_view_data_source.h>

namespace Escher {

class SelectableListViewDataSource : public SelectableTableViewDataSource {
 public:
  using SelectableTableViewDataSource::SelectableTableViewDataSource;
  void selectCell(int row) { selectRow(row); }

 private:
  using SelectableTableViewDataSource::selectCellAtLocation;
  using SelectableTableViewDataSource::selectColumn;
  using SelectableTableViewDataSource::selectedColumn;
};

}  // namespace Escher
#endif
