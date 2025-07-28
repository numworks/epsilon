#ifndef ESCHER_SELECTABLE_LIST_VIEW_DELEGATE_H
#define ESCHER_SELECTABLE_LIST_VIEW_DELEGATE_H

#include <escher/selectable_table_view_delegate.h>

namespace Escher {

class SelectableListView;

class SelectableListViewDelegate : public SelectableTableViewDelegate {
 public:
  virtual void listViewDidChangeSelectionAndDidScroll(
      SelectableListView* l, int previousRow, KDPoint previousOffset,
      bool withinTemporarySelection = false) {}

 private:
  void tableViewDidChangeSelectionAndDidScroll(
      SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
      KDPoint previousOffset, bool withinTemporarySelection) override final;
};

}  // namespace Escher
#endif
