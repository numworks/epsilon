#include "table_view_controller.h"

#include <escher/container.h>

namespace Probability {

void TableViewController::didBecomeFirstResponder() {
  if (m_seletableTableView->selectedRow() < 0) {
    m_seletableTableView->selectRow(1);
  }
  Escher::Container::activeApp()->setFirstResponder(m_seletableTableView);
}

bool Probability::TableViewController::textFieldShouldFinishEditing(
    Escher::TextField * textField,
    Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;  // TODO up and down too
}

}  // namespace Probability
