#include "store_parameter_controller.h"

#include <assert.h>

#include "../app.h"
#include "store_controller.h"

using namespace Escher;

namespace Regression {

StoreParameterController::StoreParameterController(
    Responder* parentResponder, Shared::StoreColumnHelper* storeColumnHelper)
    : Shared::StoreParameterController(parentResponder, storeColumnHelper) {
  m_changeRegressionCell.label()->setMessage(I18n::Message::RegressionModel);
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if (selectedCell() == &m_changeRegressionCell &&
      m_changeRegressionCell.canBeActivatedByEvent(event)) {
    RegressionController* regressionController =
        App::app()->regressionController();
    regressionController->setSeries(m_storeColumnHelper->selectedSeries());
    regressionController->setDisplayedFromDataTab(true);
    stackView()->push(regressionController);
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

AbstractMenuCell* StoreParameterController::cell(int row) {
  assert(row >= 0 && row < numberOfRows());
  if (row == k_changeRegressionCellIndex) {
    return &m_changeRegressionCell;
  }
  return Shared::StoreParameterController::cell(
      row - (row > k_changeRegressionCellIndex));
}

void StoreParameterController::viewWillAppear() {
  Store* regressionStore = static_cast<Store*>(m_storeColumnHelper->store());
  m_changeRegressionCell.subLabel()->setMessage(
      regressionStore->modelForSeries(m_storeColumnHelper->selectedSeries())
          ->name());
  Shared::StoreParameterController::viewWillAppear();
}

}  // namespace Regression
