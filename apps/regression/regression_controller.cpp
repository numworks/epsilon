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
#include <algorithm>

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
  int initialindex = std::max(0, static_cast<int>(m_store->seriesRegressionType(m_series))-1);
  assert(m_store->seriesRegressionType(m_series) == Model::Type::None || m_store->seriesRegressionType(m_series) == ModelTypeAtIndex(initialindex));
  selectCellAtLocation(0, initialindex);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool RegressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(m_series > -1);
    m_store->setSeriesRegressionType(m_series, ModelTypeAtIndex(selectedRow()));
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
  MessageTableCellWithExpression * castedCell = static_cast<MessageTableCellWithExpression *>(cell);
  Model * model = m_store->regressionModel(ModelTypeAtIndex(index));
  castedCell->setMessage(model->name());
  castedCell->setLayout(model->layout());
}

}
