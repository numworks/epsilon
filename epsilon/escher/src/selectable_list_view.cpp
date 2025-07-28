#include <escher/selectable_list_view.h>

namespace Escher {

void SelectableListView::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() < 0 && totalNumberOfRows() > 0) {
      selectCell(0);
    }
    SelectableTableView::handleResponderChainEvent(event);
  } else {
    SelectableTableView::handleResponderChainEvent(event);
  }
}

}  // namespace Escher
