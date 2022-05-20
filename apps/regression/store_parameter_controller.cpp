#include "store_parameter_controller.h"
#include "app.h"
#include "store_controller.h"
#include <assert.h>

using namespace Escher;

namespace Regression {

StoreParameterController::StoreParameterController(Responder * parentResponder, StoreController * storeController) :
  Shared::StoreParameterController(parentResponder, storeController),
  m_changeRegressionCell(I18n::Message::RegressionModel)
{
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right)
        && selectedRow() == indexOfRegressionCell()) {
    RegressionController * regressionController = App::app()->regressionController();
    regressionController->setSeries(m_storeController->selectedSeries());
    regressionController->setDisplayedFromDataTab(true);
    stackView()->push(regressionController);
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(index >= 0 && index < numberOfCells());
  if (index == indexOfRegressionCell()) {
    return &m_changeRegressionCell;
  }
  return Shared::StoreParameterController::reusableCell(index, type);
}

void StoreParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index == indexOfRegressionCell()) {
    assert(cell == &m_changeRegressionCell);
    m_changeRegressionCell.setSubtitle(static_cast<StoreController *>(m_storeController)->selectedModel()->name());
    return;
  }
  assert(cell != &m_changeRegressionCell);
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

}
