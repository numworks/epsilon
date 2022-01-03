#include "simple_interest_menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

using namespace Solver;

SimpleInterestMenuController::SimpleInterestMenuController(Escher::StackViewController * parentResponder, SimpleInterestController * simpleInterestController, FinanceData * data) :
      SelectableCellListPage(parentResponder), m_simpleInterestController(simpleInterestController), m_data(data) {
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
    switch (selectedRow()) {
      case k_indexOfN:
        m_data->m_data.m_simpleInterestData.m_unknown = SimpleInterestUnknown::n;
        break;
      case k_indexOfRPct:
        m_data->m_data.m_simpleInterestData.m_unknown = SimpleInterestUnknown::rPct;
        break;
      case k_indexOfP:
        m_data->m_data.m_simpleInterestData.m_unknown = SimpleInterestUnknown::P;
        break;
      case k_indexOfI:
        m_data->m_data.m_simpleInterestData.m_unknown = SimpleInterestUnknown::I;
        break;
    }
    openPage(m_simpleInterestController);
  } else if (event == Ion::Events::Left) {
    stackViewController()->pop();
  } else {
    return false;
  }
  return true;


}
