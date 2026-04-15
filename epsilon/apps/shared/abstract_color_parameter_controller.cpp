#include "abstract_color_parameter_controller.h"

#include <assert.h>

using namespace Escher;

namespace Shared {

void AbstractColorParameterController::viewWillAppear() {
  selectRow(currentColorIndex());
  SelectableListViewController<SimpleListViewDataSource>::viewWillAppear();
}

bool AbstractColorParameterController::handleEvent(Ion::Events::Event event) {
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    setColorIndex(selectedRow());
    popAfterColorSelection(stack);
    return true;
  }
  if (event == Ion::Events::Left) {
    stack->pop();
    return true;
  }
  return false;
}

void AbstractColorParameterController::fillCellForRow(HighlightCell* cell,
                                                      int row) {
  ColorCell* colorCell = static_cast<ColorCell*>(cell);
  assert(row >= 0 && row < ColorNames::k_count);
  colorCell->label()->setMessage(ColorNames::k_messages[row]);
  colorCell->accessory()->setColor(ColorNames::k_colors[row]);
}

KDCoordinate AbstractColorParameterController::defaultRowHeight() {
  ColorCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, 0);
}

}  // namespace Shared
