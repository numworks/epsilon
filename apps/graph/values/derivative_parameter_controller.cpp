#include "derivative_parameter_controller.h"
#include "values_controller.h"
#include <assert.h>

namespace Graph {

DerivativeParameterController::DerivativeParameterController(ValuesController * valuesController) :
  ViewController(valuesController),
  m_hideColumn(I18n::Message::HideDerivativeColumn),
#if COPY_COLUMN
  m_copyColumn(I18n::Message::CopyColumnInList),
#endif
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
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
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool DerivativeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    switch (selectedRow()) {
      case 0:
      {
        m_valuesController->selectCellAtLocation(m_valuesController->selectedColumn()-1, m_valuesController->selectedRow());
        m_function->setDisplayDerivative(false);
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      }
#if COPY_COLUMN
    case 1:
    /* TODO: implement function copy column */
      return true;
#endif
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
#if COPY_COLUMN
  HighlightCell * cells[] = {&m_hideColumn, &m_copyColumn};
#else
  HighlightCell * cells[] = {&m_hideColumn};
#endif
  return cells[index];
}

int DerivativeParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate DerivativeParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

}
