#include "calculation_selectable_table_view.h"

using namespace Escher;

namespace Statistics {

bool CalculationSelectableTableView::handleEvent(Ion::Events::Event & event) {
  if (event == Ion::Events::Up && selectedColumn() <= 1 && selectedRow() == 1) {
    return false;
  }
  if (event == Ion::Events::Left && selectedColumn() == 2 && selectedRow() == 0) {
    return selectCellAtLocation(0, 1);
  }
  return SelectableTableView::handleEvent(event);
}

}
