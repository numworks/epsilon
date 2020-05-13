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
  ListViewDataSource(),
  SelectableTableViewDataSource(),
  m_selectableTableView(this, this, this),
  m_store(store),
  m_series(-1)
{
}

const char * RegressionController::title() {
  return I18n::translate(I18n::Message::Regression);
}

void RegressionController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool RegressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(m_series > -1);
    m_store->setSeriesRegressionType(m_series, (Model::Type)selectedRow());
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->pop();
    stack->pop();
    return true;
  }
  if (event == Ion::Events::Left) {
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->pop();
    return true;
  }
  return false;

}
KDCoordinate RegressionController::rowHeight(int j) {
  assert (j >= 0 && j < numberOfRows());
  return j == numberOfRows() -1 ? k_logisticCellHeight : Metric::ParameterCellHeight;
}

KDCoordinate RegressionController::cumulatedHeightFromIndex(int j) {
  assert (j >= 0 && j <= numberOfRows());
  KDCoordinate result = 0;
  for (int i = 0; i < j; i++) {
    result+= rowHeight(i);
  }
  return result;
}

int RegressionController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

HighlightCell * RegressionController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_numberOfCells);
  return &m_regressionCells[index];
}

void RegressionController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  assert(i == 0);
  assert(j >= 0 && j < k_numberOfRows);
  I18n::Message messages[k_numberOfRows] = {I18n::Message::Linear, I18n::Message::Proportional, I18n::Message::Quadratic, I18n::Message::Cubic, I18n::Message::Quartic, I18n::Message::Logarithmic, I18n::Message::Exponential, I18n::Message::Power, I18n::Message::Trigonometrical, I18n::Message::Logistic};
  MessageTableCellWithExpression * castedCell = static_cast<MessageTableCellWithExpression *>(cell);
  castedCell->setMessage(messages[j]);
  castedCell->setLayout(m_store->regressionModel((Model::Type) j)->layout());
}

}
