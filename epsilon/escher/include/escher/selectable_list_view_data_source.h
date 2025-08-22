#pragma once

#include <escher/selectable_table_view_data_source.h>

namespace Escher {

class SelectableListViewDataSource : public SelectableTableViewDataSource {
 public:
  using SelectableTableViewDataSource::SelectableTableViewDataSource;

 private:
  using SelectableTableViewDataSource::selectCellAtLocation;
  using SelectableTableViewDataSource::selectColumn;
  using SelectableTableViewDataSource::selectedColumn;
};

}  // namespace Escher
