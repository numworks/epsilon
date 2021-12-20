#include "compound_interest_menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

using namespace Solver;

CompoundInterestMenuController::CompoundInterestMenuController(Escher::StackViewController * parentResponder, FinanceData * data) :
      SelectableCellListPage(parentResponder), m_data(data) {
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

void CompoundInterestMenuController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
}

bool CompoundInterestMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    ViewController * controller = nullptr;
    switch (selectedRow()) {
      case k_indexOfN:
        m_data->m_data.m_compoundInterestData.m_unknown = CompoundInterestUnknown::N;
        // TODO Hugo : Add controller
        controller = nullptr;
        break;
      case k_indexOfRPct:
        m_data->m_data.m_compoundInterestData.m_unknown = CompoundInterestUnknown::rPct;
        // TODO Hugo : Add controller
        controller = nullptr;
        break;
      case k_indexOfPV:
        m_data->m_data.m_compoundInterestData.m_unknown = CompoundInterestUnknown::PV;
        // TODO Hugo : Add controller
        controller = nullptr;
        break;
      case k_indexOfPmt:
        m_data->m_data.m_compoundInterestData.m_unknown = CompoundInterestUnknown::Pmt;
        // TODO Hugo : Add controller
        controller = nullptr;
        break;
      case k_indexOfFV:
        m_data->m_data.m_compoundInterestData.m_unknown = CompoundInterestUnknown::FV;
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
