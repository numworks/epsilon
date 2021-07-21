#include "table_view_controller.h"

#include <escher/container.h>

namespace Probability {

void TableViewController::didBecomeFirstResponder() {
  if (selectableTableView()->selectedRow() < 0) {
    selectableTableView()->selectRow(1);
  }
  if (selectableTableView()->selectedColumn() < 0) {
    selectableTableView()->selectColumn(1);
  }
  Escher::Container::activeApp()->setFirstResponder(selectableTableView());
}

bool Probability::TableViewController::textFieldShouldFinishEditing(
    Escher::TextField * textField,
    Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;  // TODO up and down too
}

}  // namespace Probability
