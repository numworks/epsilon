#include "store_parameter_controller.h"
#include "app.h"
#include "store_controller.h"
#include <assert.h>

namespace Regression {

StoreParameterController::StoreParameterController(Responder * parentResponder, Store * store, StoreController * storeController) :
  Shared::StoreParameterController(parentResponder, store, storeController),
  m_changeRegressionCell(I18n::Message::Regression),
  m_lastSelectionIsRegression(false)
{
}

void StoreParameterController::viewWillAppear() {
  m_selectableTableView.reloadData();
  Shared::StoreParameterController::viewWillAppear();
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
  assert(index < reusableCellCount(type));
  if (type == k_regressionCellType) {
    assert(index == 0);
    return &m_changeRegressionCell;
  }
  return Shared::StoreParameterController::reusableCell(index, type);
}
KDCoordinate StoreParameterController::rowHeight(int j) {
  if (j == numberOfRows() - 1) {
    if (static_cast<Store *>(m_store)->seriesRegressionType(m_series) == Model::Type::Logistic) {
      return RegressionController::k_logisticCellHeight;
    }
    return Metric::ParameterCellHeight;
  }
  return Shared::StoreParameterController::rowHeight(j);
}

int StoreParameterController::reusableCellCount(int type) {
  if (type == k_regressionCellType) {
    return 1;
  }
  return Shared::StoreParameterController::reusableCellCount(type);
}

int StoreParameterController::typeAtLocation(int i, int j) {
  assert(i == 0);
  if (j == numberOfRows() -1) {
    return k_regressionCellType;
  }
  return Shared::StoreParameterController::typeAtLocation(i, j);
}

void StoreParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows() -1) {
    m_changeRegressionCell.setLayout(static_cast<Store *>(m_store)->modelForSeries(m_series)->layout());
    return;
  }
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

}
