#include "derivative_parameter_controller.h"
#include "values_controller.h"
#include <assert.h>

namespace Graph {

DerivativeParameterController::DerivativeParameterController(ValuesController * valuesController) :
  ViewController(valuesController),
  m_hideColumn(PointerTableCell(I18n::Message::HideDerivativeColumn)),
  m_copyColumn(PointerTableCellWithChevron(I18n::Message::CopyColumnInList)),
  m_selectableTableView(SelectableTableView(this, this, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)),
  m_function(nullptr),
  m_valuesController(valuesController)
{
}

const char * DerivativeParameterController::title() {
  strlcpy(m_pageTitle, I18n::translate(I18n::Message::DerivativeColumn), k_maxNumberOfCharsInTitle);
  for (int currentChar = 0; currentChar < k_maxNumberOfCharsInTitle; currentChar++) {
    if (m_pageTitle[currentChar] == '(') {
      m_pageTitle[currentChar-2] = *m_function->name();
      break;
    }
  }
  return m_pageTitle;
}

View * DerivativeParameterController::view() {
  return &m_selectableTableView;
}

void DerivativeParameterController::setFunction(CartesianFunction * function) {
  m_function = function;
}

void DerivativeParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool DerivativeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (m_selectableTableView.selectedRow()) {
      case 0:
      {
        m_valuesController->selectCellAtLocation(m_valuesController->activeColumn()-1, m_valuesController->activeRow());
        m_function->setDisplayDerivative(false);
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      }
      case 1:
        return false;
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

int DerivativeParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * DerivativeParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_hideColumn, &m_copyColumn};
  return cells[index];
}

int DerivativeParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate DerivativeParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

}
