#include <escher/selectable_list_view.h>

namespace Escher {

void SelectableListView::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCell(0);
  }
  SelectableTableView::didBecomeFirstResponder();
}

}  // namespace Escher
