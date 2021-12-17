#include "finance_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

using namespace Solver;

FinanceController::FinanceController(Escher::StackViewController * parentResponder, SimpleInterestController * simpleInterestController, CompoundInterestController * compoundInterestController) :
      SelectableCellListPage(parentResponder),
      m_simpleInterestController(simpleInterestController),
      m_compoundInterestController(compoundInterestController) {
  selectRow(0);
  m_cells[k_indexOfSimpleIntereset].setMessage(I18n::Message::SimpleInterest);
  m_cells[k_indexOfSimpleIntereset].setSubtitle(I18n::Message::SimpleInterestDescription);
  m_cells[k_indexOfCompoundInterest].setMessage(I18n::Message::CompoundInterest);
  m_cells[k_indexOfCompoundInterest].setSubtitle(I18n::Message::CompoundInterestDescription);
}

const char * FinanceController::title() {
  return I18n::translate(I18n::Message::FinanceSubAppTitle);
}

void FinanceController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
}

bool FinanceController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    ViewController * controller = nullptr;
    switch (selectedRow()) {
      case k_indexOfSimpleIntereset:
        controller = m_simpleInterestController;
        break;
      case k_indexOfCompoundInterest:
        controller = m_compoundInterestController;
        break;
    }
    assert(controller != nullptr);
    openPage(controller);
  } else if (event == Ion::Events::Left) {
    stackViewController()->pop();
  } else {
    return false;
  }
  return true;


}
