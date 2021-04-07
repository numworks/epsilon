#include "store_parameter_controller.h"
#include "app.h"
#include "store_controller.h"
#include <assert.h>

using namespace Escher;

namespace Regression {

StoreParameterController::StoreParameterController(Responder * parentResponder, Store * store, StoreController * storeController) :
  Shared::StoreParameterController(parentResponder, store, storeController),
  m_changeRegressionCell(I18n::Message::Regression),
  m_lastSelectionIsRegression(false)
{
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) && selectedRow() == numberOfRows() - 1) {
    RegressionController * regressionController = App::app()->regressionController();
    regressionController->setSeries(m_series);
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->push(regressionController);
    m_lastSelectionIsRegression = true;
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

void StoreParameterController::didBecomeFirstResponder() {
  if (m_lastSelectionIsRegression) {
    selectCellAtLocation(0, 2);
  } else {
    selectCellAtLocation(0, 0);
  }
  m_lastSelectionIsRegression = false;
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == k_regressionCellType) {
    return &m_changeRegressionCell;
  }
  return Shared::StoreParameterController::reusableCell(index, type);
}

int StoreParameterController::reusableCellCount(int type) {
  if (type == k_regressionCellType) {
    return 1;
  }
  return Shared::StoreParameterController::reusableCellCount(type);
}

int StoreParameterController::typeAtIndex(int index) {
  if (index == numberOfRows() - 1) {
    return k_regressionCellType;
  }
  return Shared::StoreParameterController::typeAtIndex(index);
}

void StoreParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows() - 1) {
    assert(cell == &m_changeRegressionCell);
    m_changeRegressionCell.setLayout(static_cast<Store *>(m_store)->modelForSeries(m_series)->layout());
    return;
  }
  assert(cell != &m_changeRegressionCell);
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

}
