#include "initialisation_parameter_controller.h"
#include <assert.h>

using namespace Shared;

namespace Regression {

InitialisationParameterController::InitialisationParameterController(Responder * parentResponder, Store * store) :
  ViewController(parentResponder),
  m_selectableTableView(this),
  m_store(store)
{
}

const char * InitialisationParameterController::title() {
  return I18n::translate(I18n::Message::Initialization);
}

View * InitialisationParameterController::view() {
  return &m_selectableTableView;
}

void InitialisationParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool InitialisationParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    RangeMethodPointer rangeMethods[k_totalNumberOfCells] = {&InteractiveCurveViewRange::roundAbscissa,
      &InteractiveCurveViewRange::normalize, &InteractiveCurveViewRange::setDefault};
    (m_store->*rangeMethods[selectedRow()])();
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->pop();
    return true;
  }
  return false;
}

int InitialisationParameterController::numberOfRows() {
  return k_totalNumberOfCells;
};


HighlightCell * InitialisationParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int InitialisationParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

KDCoordinate InitialisationParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void InitialisationParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message titles[3] = {I18n::Message::RoundAbscissa, I18n::Message::Orthonormal, I18n::Message::DefaultSetting};
  myCell->setMessage(titles[index]);
}

}
