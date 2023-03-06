#include <escher/selectable_list_view.h>
#include <escher/selectable_list_view_delegate.h>

namespace Escher {

/* These methods are in the cpp to avoid a dependency loop between delegates and
 * views. */

void SelectableListViewDelegate::tableViewDidChangeSelection(
    SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
    bool withinTemporarySelection) {
  return listViewDidChangeSelection(static_cast<SelectableListView*>(t),
                                    previousSelectedRow,
                                    withinTemporarySelection);
}

void SelectableListViewDelegate::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
    bool withinTemporarySelection) {
  return listViewDidChangeSelectionAndDidScroll(
      static_cast<SelectableListView*>(t), previousSelectedRow,
      withinTemporarySelection);
}

bool SelectableListViewDelegate::canStoreContentOfCellAtLocation(
    SelectableTableView* t, int col, int row) const {
  return canStoreContentOfCell(static_cast<SelectableListView*>(t), row);
};

}  // namespace Escher
