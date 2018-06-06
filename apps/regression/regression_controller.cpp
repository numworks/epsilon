#include "regression_controller.h"
#include "model/cubic_model.h"
#include "model/exponential_model.h"
#include "model/linear_model.h"
#include "model/logarithmic_model.h"
#include "model/logistic_model.h"
#include "model/power_model.h"
#include "model/quadratic_model.h"
#include "model/quartic_model.h"
#include "model/trigonometric_model.h"
#include <assert.h>

using namespace Poincare;

namespace Regression {

RegressionController::RegressionController(Responder * parentResponder, Store * store) :
  ViewController(parentResponder),
  SimpleListViewDataSource(),
  SelectableTableViewDataSource(),
  m_selectableTableView(this, this, this),
  m_store(store),
  m_series(-1)
{
}

const char * RegressionController::title() {
  return I18n::translate(I18n::Message::RegressionChoice);
}

void RegressionController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool RegressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(m_series > -1);
    m_store->setSeriesRegressionType(m_series, (Model::Type)selectedRow());
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->pop();
    return true;
  }
  return false;
}

HighlightCell * RegressionController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_numberOfCells);
  return &m_regressionCells[index];
}

void RegressionController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  assert(i == 0);
  assert(j >= 0 && j < k_numberOfRows);
  I18n::Message messages[k_numberOfRows] = {I18n::Message::Linear, I18n::Message::Quadratic, I18n::Message::Cubic, I18n::Message::Quartic, I18n::Message::Logarithmic, I18n::Message::Exponential, I18n::Message::Power, I18n::Message::Trigonometric, I18n::Message::Logistic};
  MessageTableCellWithExpression * castedCell = static_cast<MessageTableCellWithExpression *>(cell);
  castedCell->setMessage(messages[j]);
  switch ((Model::Type) j) {
    // TODO virtualize ?
    case Model::Type::Linear:
      castedCell->setExpressionLayout(LinearModel::Layout());
      break;
    case Model::Type::Quadratic:
      castedCell->setExpressionLayout(QuadraticModel::Layout());
      break;
    case Model::Type::Cubic:
      castedCell->setExpressionLayout(CubicModel::Layout());
      break;
    case Model::Type::Quartic:
      castedCell->setExpressionLayout(QuarticModel::Layout());
      break;
    case Model::Type::Logarithmic:
      castedCell->setExpressionLayout(LogarithmicModel::Layout());
      break;
    case Model::Type::Exponential:
      castedCell->setExpressionLayout(ExponentialModel::Layout());
      break;
    case Model::Type::Power:
      castedCell->setExpressionLayout(PowerModel::Layout());
      break;
    case Model::Type::Trigonometric:
      castedCell->setExpressionLayout(TrigonometricModel::Layout());
      break;
    case Model::Type::Logistic:
      castedCell->setExpressionLayout(LogisticModel::Layout());
      break;
    default:
      assert(false);
      break;
  }
}

}
