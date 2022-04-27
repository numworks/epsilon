#include <escher/container.h>
#include <escher/stack_view_controller.h>
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
#include "model/median_model.h"
#include "../shared/interactive_curve_view_controller.h"
#include <assert.h>

using namespace Poincare;
using namespace Escher;

namespace Regression {

RegressionController::RegressionController(Responder * parentResponder, Store * store) :
  SelectableListViewController(parentResponder),
  m_store(store),
  m_series(-1)
{
}

const char * RegressionController::title() {
  return I18n::translate(I18n::Message::Regression);
}

void RegressionController::didBecomeFirstResponder() {
  selectCellAtLocation(0, static_cast<uint8_t>(m_store->seriesRegressionType(m_series)));
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool RegressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(m_series > -1);
    m_store->setSeriesRegressionType(m_series, (Model::Type)selectedRow());
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->popUntilDepth(Shared::InteractiveCurveViewController::k_graphControllerStackDepth, true);
    return true;
  }
  if (event == Ion::Events::Left) {
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->pop();
    return true;
  }
  return false;

}
KDCoordinate RegressionController::nonMemoizedRowHeight(int j) {
  assert (j >= 0 && j < numberOfRows());
  MessageTableCellWithExpression tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, j);
}

HighlightCell * RegressionController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_numberOfCells);
  return &m_regressionCells[index];
}

void RegressionController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(index >= 0 && index < k_numberOfRows);
  I18n::Message messages[k_numberOfRows] = {I18n::Message::Linear, I18n::Message::Proportional, I18n::Message::Quadratic, I18n::Message::Cubic, I18n::Message::Quartic, I18n::Message::Logarithmic, I18n::Message::Exponential, I18n::Message::Exponential, I18n::Message::Power, I18n::Message::Trigonometrical, I18n::Message::Logistic, I18n::Message::MedianRegression};
  // The exponential model has two forms corresponding to two rows
  static_assert(sizeof(messages) / sizeof(I18n::Message) == Model::k_numberOfModels + 1, "Inconsistency between the number of models and the number of messages");
  MessageTableCellWithExpression * castedCell = static_cast<MessageTableCellWithExpression *>(cell);
  castedCell->setMessage(messages[index]);
  castedCell->setLayout(m_store->regressionModel((Model::Type) index)->layout());
}

}
