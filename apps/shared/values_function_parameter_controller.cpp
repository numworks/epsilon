#include "values_function_parameter_controller.h"
#include <assert.h>

namespace Shared {

ValuesFunctionParameterController::ValuesFunctionParameterController(char symbol) :
  ViewController(nullptr),
  m_copyColumn(PointerTableCellWithChevron(I18n::Message::CopyColumnInList)),
  m_selectableTableView(SelectableTableView(this, this, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)),
  m_function(nullptr),
  m_symbol(symbol)
{
}

const char * ValuesFunctionParameterController::title() {
  strlcpy(m_pageTitle, I18n::translate(I18n::Message::FunctionColumn), k_maxNumberOfCharsInTitle);
  for (int currentChar = 0; currentChar < k_maxNumberOfCharsInTitle; currentChar++) {
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
  m_selectableTableView.selectCellAtLocation(0, 0);
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

