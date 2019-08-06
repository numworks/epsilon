#include "initialisation_parameter_controller.h"
#include <assert.h>
#include <cmath>

namespace Shared {

View * InitialisationParameterController::view() {
  return &m_selectableTableView;
}

const char * InitialisationParameterController::title() {
  return I18n::translate(I18n::Message::Initialization);
}

bool InitialisationParameterController::handleEvent(Ion::Events::Event event) {
if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    RangeMethodPointer rangeMethods[k_totalNumberOfCells] = {
      &InteractiveCurveViewRange::setTrigonometric,
      &InteractiveCurveViewRange::roundAbscissa,
      &InteractiveCurveViewRange::normalize,
      &InteractiveCurveViewRange::setDefault};
    (m_graphRange->*rangeMethods[selectedRow()])();
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->pop();
    return true;
  }
  return false;
}

void InitialisationParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

int InitialisationParameterController::numberOfRows() {
  return k_totalNumberOfCells;
}

KDCoordinate InitialisationParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

HighlightCell * InitialisationParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int InitialisationParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

void InitialisationParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  I18n::Message titles[4] = {
    I18n::Message::Trigonometric,
    I18n::Message::RoundAbscissa,
    I18n::Message::Orthonormal,
    I18n::Message::DefaultSetting};
  ((MessageTableCell *)cell)->setMessage(titles[index]);
}

}
