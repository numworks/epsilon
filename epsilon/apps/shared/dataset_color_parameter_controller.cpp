#include "dataset_color_parameter_controller.h"

#include <assert.h>
#include <escher/stack_view_controller.h>

using namespace Escher;

namespace Shared {

DatasetColorParameterController::DatasetColorParameterController(
    Responder* parentResponder)
    : SelectableListViewController<SimpleListViewDataSource>(parentResponder),
      m_store(nullptr),
      m_series(0) {}

void DatasetColorParameterController::viewWillAppear() {
  assert(m_store != nullptr);
  selectRow(m_store->colorIndexForSeries(m_series));
  SelectableListViewController<SimpleListViewDataSource>::viewWillAppear();
}

bool DatasetColorParameterController::handleEvent(Ion::Events::Event event) {
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(m_store != nullptr);
    m_store->setColorIndexForSeries(m_series,
                                    static_cast<uint8_t>(selectedRow()));
    stack->pop();
    return true;
  }
  if (event == Ion::Events::Left) {
    stack->pop();
    return true;
  }
  return false;
}

void DatasetColorParameterController::fillCellForRow(HighlightCell* cell,
                                                     int row) {
  ColorCell* colorCell = static_cast<ColorCell*>(cell);
  assert(row >= 0 && row < ColorNames::k_count);
  colorCell->label()->setMessage(ColorNames::k_messages[row]);
  colorCell->accessory()->setColor(ColorNames::k_colors[row]);
}

KDCoordinate DatasetColorParameterController::defaultRowHeight() {
  ColorCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, 0);
}

}  // namespace Shared
