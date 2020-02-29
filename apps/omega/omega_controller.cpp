#include "omega_controller.h"
#include <assert.h>

namespace Omega {

OmegaController::OmegaController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(parentResponder, this, this, this),
  m_cells{}
{
  for (int i = 0; i < k_numberOfCells; i++) {
    //m_cells[i].setParentResponder(&m_selectableTableView);
    //m_cells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

View * OmegaController::view() {
  return &m_omegaView;
}

void OmegaController::didBecomeFirstResponder() {
}

bool OmegaController::handleEvent(Ion::Events::Event event) {
  /* if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return true;
  } */
  return false;
}

int OmegaController::numberOfRows() const {
  return 2;
}

KDCoordinate OmegaController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

HighlightCell * OmegaController::reusableCell(int index, int type) {
  assert(index >= 0 && index <= 1);
  return &m_cells[index];
}

int OmegaController::reusableCellCount(int type) {
  return 1;
}

int OmegaController::typeAtLocation(int i, int j) {
  return 0;
}

}
