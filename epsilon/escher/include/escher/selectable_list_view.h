#ifndef ESCHER_SELECTABLE_LIST_VIEW_H
#define ESCHER_SELECTABLE_LIST_VIEW_H

#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view_data_source.h>
#include <escher/selectable_list_view_delegate.h>
#include <escher/selectable_table_view.h>

namespace Escher {

class SelectableListView : public SelectableTableView {
 public:
  SelectableListView(
      Responder* parentResponder, ListViewDataSource* dataSource,
      SelectableListViewDataSource* selectionDataSource = nullptr,
      SelectableListViewDelegate* delegate = nullptr)
      : SelectableTableView(parentResponder, dataSource, selectionDataSource,
                            delegate) {}

  void scrollToCell(int row) { return scrollToCell(0, row); }
  void selectCell(int row) { selectCellAtLocation(0, row); }
  void reloadCell(int row, bool forceSetFrame = false) {
    reloadCellAtLocation(0, row, forceSetFrame);
  }
  void reloadSelectedCell(bool forceSetFrame = false) {
    reloadCellAtLocation(0, selectedRow(), forceSetFrame);
  }
  HighlightCell* cell(int row) { return cellAtLocation(0, row); }
  void setDelegate(SelectableListViewDelegate* delegate) {
    m_delegate = delegate;
  }

  void layoutSubviews(bool force = false) override {
    static_cast<ListViewDataSource*>(dataSource())->initWidth(this);
    SelectableTableView::layoutSubviews(force);
  }

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  // Hide column-related methods
  using SelectableTableView::selectCellAtLocation;
  using SelectableTableView::selectColumn;
  using SelectableTableView::selectedColumn;
  using TableView::cellAtLocation;
  using TableView::firstDisplayedColumn;
  using TableView::numberOfDisplayableColumns;
  using TableView::reloadCellAtLocation;
  using TableView::scrollToCell;
};

}  // namespace Escher
#endif
