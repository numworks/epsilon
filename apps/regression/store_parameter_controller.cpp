#include "store_parameter_controller.h"
#include "app.h"
#include "store_controller.h"
#include <assert.h>

using namespace Escher;

namespace Regression {

StoreParameterController::StoreParameterController(Responder * parentResponder, StoreController * storeController) :
  Shared::StoreParameterController(parentResponder, storeController),
  m_changeRegressionCell(I18n::Message::Regression)
{
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right)
        && selectedRow() == k_indexOfRegressionCell) {
    RegressionController * regressionController = App::app()->regressionController();
    regressionController->setSeries(m_storeController->selectedSeries());
    stackView()->push(regressionController);
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}
KDCoordinate StoreParameterController::nonMemoizedRowHeight(int index) {
  if (typeAtIndex(index) == k_regressionCellType) {
    return heightForCellAtIndex(&m_changeRegressionCell, index);
  }
  return Shared::StoreParameterController::nonMemoizedRowHeight(index);
}

HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == k_regressionCellType) {
    return &m_changeRegressionCell;
  }
  return Shared::StoreParameterController::reusableCell(index, type);
}

void StoreParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index == k_indexOfRegressionCell) {
    assert(cell == &m_changeRegressionCell);
    m_changeRegressionCell.setLayout(static_cast<StoreController *>(m_storeController)->model()->layout());
    return;
  }
  assert(cell != &m_changeRegressionCell);
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

}
