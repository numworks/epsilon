#include "calculation_parameter_controller.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Graph {

CalculationParameterController::CalculationParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_function(nullptr)
{
}

const char * CalculationParameterController::title() {
  return I18n::translate(I18n::Message::Compute);
}

View * CalculationParameterController::view() {
  return &m_selectableTableView;
}

void CalculationParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool CalculationParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return true;
  }
  return false;
}

int CalculationParameterController::numberOfRows() {
  return k_totalNumberOfCells;
};


HighlightCell * CalculationParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int CalculationParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

KDCoordinate CalculationParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void CalculationParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message titles[k_totalNumberOfCells] = {I18n::Message::Intersection, I18n::Message::Maximum, I18n::Message::Minimum, I18n::Message::Zeros, I18n::Message::Tangent, I18n::Message::Integral};
  myCell->setMessage(titles[index]);
}

void CalculationParameterController::setFunction(Function * function) {
  m_function = function;
}

}
