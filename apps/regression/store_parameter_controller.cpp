#include "store_parameter_controller.h"
#include "store_controller.h"
#include <assert.h>

namespace Regression {

StoreParameterController::StoreParameterController(Responder * parentResponder, Store * store, StoreController * storeController) :
  Shared::StoreParameterController(parentResponder, store, storeController),
  m_changeRegressionCell(I18n::Message::ChangeRegression),
  m_regressionController(this, store)
{
  static_cast<ExpressionView *>(m_changeRegressionCell.subAccessoryView())->setHorizontalMargin(5);
}

void StoreParameterController::viewWillAppear() {
  m_selectableTableView.reloadData();
  Shared::StoreParameterController::viewWillAppear();
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) && selectedRow() == numberOfRows() - 1) {
    m_regressionController.setSeries(m_series);
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->push(&m_regressionController);
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
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
    m_changeRegressionCell.setExpressionLayout(static_cast<Store *>(m_store)->modelForSeries(m_series)->layout());
  }
  Shared::StoreParameterController::willDisplayCellForIndex(cell, index);
}

}
