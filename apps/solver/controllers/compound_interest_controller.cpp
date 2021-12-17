#include "compound_interest_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

using namespace Solver;

CompoundInterestController::CompoundInterestController(Escher::StackViewController * parentResponder) :
      SelectableCellListPage(parentResponder) {
  selectRow(0);
  m_cells[k_indexOfN].setMessage(I18n::Message::FinanceN);
  m_cells[k_indexOfN].setSubtitle(I18n::Message::NumberOfPeriods);
  m_cells[k_indexOfRPct].setMessage(I18n::Message::FinanceRPct);
  m_cells[k_indexOfRPct].setSubtitle(I18n::Message::NominalAnnualInterestRate);
  m_cells[k_indexOfPV].setMessage(I18n::Message::FinancePV);
  m_cells[k_indexOfPV].setSubtitle(I18n::Message::PresentValue);
  m_cells[k_indexOfPmt].setMessage(I18n::Message::FinancePmt);
  m_cells[k_indexOfPmt].setSubtitle(I18n::Message::PaymentEachPeriod);
  m_cells[k_indexOfFV].setMessage(I18n::Message::FinanceFV);
  m_cells[k_indexOfFV].setSubtitle(I18n::Message::FutureValue);
}

void CompoundInterestController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
}

bool CompoundInterestController::handleEvent(Ion::Events::Event event) {
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
      case k_indexOfPV:
        // TODO Hugo : Add controller
        controller = nullptr;
        break;
      case k_indexOfPmt:
        // TODO Hugo : Add controller
        controller = nullptr;
        break;
      case k_indexOfFV:
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
