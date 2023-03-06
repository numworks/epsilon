#ifndef ESCHER_SELECTABLE_LIST_VIEW_H
#define ESCHER_SELECTABLE_LIST_VIEW_H

#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view_data_source.h>
#include <escher/selectable_table_view.h>

namespace Escher {

class SelectableListView : public SelectableTableView {
 public:
  SelectableListView(
      Responder* parentResponder, ListViewDataSource* dataSource,
      SelectableListViewDataSource* selectionDataSource = nullptr,
      SelectableTableViewDelegate* delegate = nullptr)
      : SelectableTableView(parentResponder, dataSource, selectionDataSource,
                            delegate) {}

  void scrollToCell(int row) { return scrollToCell(0, row); }
  bool selectCell(int row) { return selectCellAtLocation(0, row); }
  void reloadCell(int row) { return reloadCellAtLocation(0, row); }

 private:
  // Hide column-related methods
  using SelectableTableView::selectCellAtClippedLocation;
  using SelectableTableView::selectCellAtLocation;
  using SelectableTableView::selectColumn;
  using SelectableTableView::selectedColumn;
  using TableView::cellAtLocation;
  using TableView::firstDisplayedColumnIndex;
  using TableView::numberOfDisplayableColumns;
  using TableView::reloadCellAtLocation;
  using TableView::scrollToCell;
};

}  // namespace Escher
#endif
