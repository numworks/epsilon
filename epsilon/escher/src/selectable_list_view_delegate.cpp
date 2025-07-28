#include <escher/selectable_list_view.h>
#include <escher/selectable_list_view_delegate.h>

namespace Escher {

/* These methods are in the cpp to avoid a dependency loop between delegates and
 * views. */

void SelectableListViewDelegate::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
    KDPoint previousOffset, bool withinTemporarySelection) {
  return listViewDidChangeSelectionAndDidScroll(
      static_cast<SelectableListView*>(t), previousSelectedRow, previousOffset,
      withinTemporarySelection);
}

}  // namespace Escher
