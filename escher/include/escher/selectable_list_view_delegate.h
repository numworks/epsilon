#ifndef ESCHER_SELECTABLE_LIST_VIEW_DELEGATE_H
#define ESCHER_SELECTABLE_LIST_VIEW_DELEGATE_H

#include <escher/selectable_table_view_delegate.h>

namespace Escher {

class SelectableListView;

class SelectableListViewDelegate : public SelectableTableViewDelegate {
 public:
  virtual void listViewDidChangeSelection(
      SelectableListView* l, int previousRow,
      bool withinTemporarySelection = false) {}
  virtual void listViewDidChangeSelectionAndDidScroll(
      SelectableListView* l, int previousRow,
      bool withinTemporarySelection = false) {}
  virtual bool canStoreContentOfCell(SelectableListView* l, int row) const {
    return true;
  }

 private:
  void tableViewDidChangeSelection(
      SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
      bool withinTemporarySelection) override final;
  void tableViewDidChangeSelectionAndDidScroll(
      SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
      bool withinTemporarySelection) override final;
  bool canStoreContentOfCellAtLocation(SelectableTableView* t, int col,
                                       int row) const override final;
};

}  // namespace Escher
#endif
