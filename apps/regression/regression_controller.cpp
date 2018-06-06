#include "regression_controller.h"
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
  /*// aX+b
  m_regressionLayouts[0] = LayoutEngine::createStringLayout("aX+b");
  // aX^2+bX+c
  const ExpressionLayout * const * quadraticLayoutOperands[] = {

  };
  m_regressionLayouts[1]
  // aX^3+bX^2+cX+d
  //TODO
  // aX^4+bX^3+cX^2+dX+e
  //TODO
  // aln(X)+b
  m_regressionLayouts[4] = LayoutEngine::createStringLayout("aln(X)+b");
  // ae^(bX)
  //TODO
  //aX^b
  //TODO
  // asin(bX+c)
  m_regressionLayouts[7] = LayoutEngine::createStringLayout("asin(bX+c)");
  // c/(1+ae^(-bX)
  //TODO*/
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
}

}
