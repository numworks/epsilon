#include "values_function_parameter_controller.h"
#include <assert.h>

namespace Shared {

ValuesFunctionParameterController::ValuesFunctionParameterController(char symbol) :
  ViewController(nullptr),
  m_copyColumn(I18n::Message::CopyColumnInList),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_function(nullptr),
  m_symbol(symbol)
{
}

const char * ValuesFunctionParameterController::title() {
  strlcpy(m_pageTitle, I18n::translate(I18n::Message::FunctionColumn), k_maxNumberOfCharsInTitle);
  for (int currentChar = 0; currentChar < k_maxNumberOfCharsInTitle-1; currentChar++) {
    if (m_pageTitle[currentChar] == '(') {
      m_pageTitle[currentChar-1] = *m_function->name();
      m_pageTitle[currentChar+1] = m_symbol;
      break;
    }
  }
  return m_pageTitle;
}

View * ValuesFunctionParameterController::view() {
  return &m_selectableTableView;
}

void ValuesFunctionParameterController::setFunction(Function * function) {
  m_function = function;
}

void ValuesFunctionParameterController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

int ValuesFunctionParameterController::numberOfRows() {
  return 1;
};

HighlightCell * ValuesFunctionParameterController::reusableCell(int index) {
  assert(index == 0);
  return &m_copyColumn;
}

int ValuesFunctionParameterController::reusableCellCount() {
  return 1;
}

KDCoordinate ValuesFunctionParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

}

