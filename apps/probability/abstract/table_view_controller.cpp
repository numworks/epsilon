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

void TableViewController::moveSelectionForEvent(Ion::Events::Event event,
                                                int * selectedRow,
                                                int * selectedColumn) {
  if (event == Ion::Events::Left) {
    (*selectedColumn)--;
  } else if (event == Ion::Events::Right) {
    (*selectedColumn)++;
  } else if (event == Ion::Events::Up) {
    (*selectedRow)--;
  } else if (event == Ion::Events::Down) {
    (*selectedRow)++;
  }
}

bool Probability::TableViewController::textFieldShouldFinishEditing(Escher::TextField * textField,
                                                                    Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE ||
         (event == Ion::Events::Right &&
          selectableTableView()->selectedColumn() < tableViewDataSource()->numberOfColumns() - 1) ||
         (event == Ion::Events::Left && selectableTableView()->selectedColumn() > 0) ||
         (event == Ion::Events::Down &&
          selectableTableView()->selectedRow() < tableViewDataSource()->numberOfRows() - 1) ||
         (event == Ion::Events::Up && selectableTableView()->selectedRow() > 0);
}

}  // namespace Probability
