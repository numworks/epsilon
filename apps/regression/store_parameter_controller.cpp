#include "store_parameter_controller.h"
#include "app.h"
#include "store_controller.h"
#include <assert.h>

using namespace Escher;

namespace Regression {

StoreParameterController::StoreParameterController(Responder * parentResponder, Shared::StoreColumnHelper * storeColumnHelper) :
  Shared::StoreParameterController(parentResponder, storeColumnHelper),
  m_changeRegressionCell(I18n::Message::RegressionModel)
{
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right)
        && typeAtIndex(selectedRow()) == k_changeRegressionCellType) {
    RegressionController * regressionController = App::app()->regressionController();
    regressionController->setSeries(m_storeColumnHelper->selectedSeries());
    regressionController->setDisplayedFromDataTab(true);
    stackView()->push(regressionController);
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

int StoreParameterController::typeAtIndex(int index) {
  if (index == k_changeRegressionCellIndex) {
    return k_changeRegressionCellType;
  }
  return Shared::StoreParameterController::typeAtIndex(index - (index > k_changeRegressionCellIndex));
}

HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(index >= 0 && index < numberOfRows());
  if (type == k_changeRegressionCellType) {
    return &m_changeRegressionCell;
  }
  return Shared::StoreParameterController::reusableCell(index, type);
}

void StoreParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_changeRegressionCellType) {
    assert(cell == &m_changeRegressionCell);
    Store * regressionStore = static_cast<Store *>(m_storeColumnHelper->store());
    m_changeRegressionCell.setSubtitle(regressionStore->modelForSeries(m_storeColumnHelper->selectedSeries())->name());
    return;
  }
  assert(cell != &m_changeRegressionCell);
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

}
