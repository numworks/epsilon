#include <escher/selectable_list_view_controller.h>

namespace Escher {

void SelectableViewController::centerTable(KDCoordinate availableHeight) {
  SelectableTableView* table = static_cast<SelectableTableView*>(view());
  // Give a maximum of space to the table to let it compute its preferred size
  table->margins()->setTop(0);
  table->margins()->setBottom(0);
  view()->setChildFrame(
      table, KDRect(0, 0, Ion::Display::Width, availableHeight), true);
  KDCoordinate tableHeight = table->minimalSizeForOptimalDisplay().height();
  assert(tableHeight < availableHeight);
  table->margins()->setTop((availableHeight - tableHeight) / 2);
}

}  // namespace Escher
