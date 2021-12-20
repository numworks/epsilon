#include "simple_interest_menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

using namespace Solver;

SimpleInterestMenuController::SimpleInterestMenuController(Escher::StackViewController * parentResponder) :
      SelectableCellListPage(parentResponder) {
  selectRow(0);
  m_cells[k_indexOfN].setMessage(I18n::Message::FinanceLowerN);
  m_cells[k_indexOfN].setSubtitle(I18n::Message::NumberOfDays);
  m_cells[k_indexOfRPct].setMessage(I18n::Message::FinanceRPct);
  m_cells[k_indexOfRPct].setSubtitle(I18n::Message::AnnualInterestRate);
  m_cells[k_indexOfP].setMessage(I18n::Message::FinanceP);
  m_cells[k_indexOfP].setSubtitle(I18n::Message::InitialPrincipal);
  m_cells[k_indexOfI].setMessage(I18n::Message::FinanceI);
  m_cells[k_indexOfI].setSubtitle(I18n::Message::FinalInterestAmount);
}

void SimpleInterestMenuController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
}

bool SimpleInterestMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    ViewController * controller = nullptr;
    switch (selectedRow()) {
      case k_indexOfN:
        // TODO Hugo : Add controller
        controller = nullptr;
        break;
      case k_indexOfRPct:
        // TODO Hugo : Add controller
        controller = nullptr;
        break;
      case k_indexOfP:
        // TODO Hugo : Add controller
        controller = nullptr;
        break;
      case k_indexOfI:
        // TODO Hugo : Add controller
        controller = nullptr;
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
