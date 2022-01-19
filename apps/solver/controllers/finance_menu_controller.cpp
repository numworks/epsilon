#include "finance_menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

using namespace Solver;

FinanceMenuController::FinanceMenuController(Escher::StackViewController * parentResponder, SimpleInterestMenuController * simpleInterestMenuController, CompoundInterestMenuController * compoundInterestMenuController, FinanceData * data) :
      Shared::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfFinanceCells>(parentResponder),
      m_simpleInterestMenuController(simpleInterestMenuController),
      m_compoundInterestMenuController(compoundInterestMenuController),
      m_data(data) {
  selectRow(0);
  m_cells[k_indexOfSimpleIntereset].setMessage(I18n::Message::SimpleInterest);
  m_cells[k_indexOfSimpleIntereset].setSubtitle(I18n::Message::SimpleInterestDescription);
  m_cells[k_indexOfCompoundInterest].setMessage(I18n::Message::CompoundInterest);
  m_cells[k_indexOfCompoundInterest].setSubtitle(I18n::Message::CompoundInterestDescription);
}

const char * FinanceMenuController::title() {
  return I18n::translate(I18n::Message::FinanceSubAppTitle);
}

void FinanceMenuController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
}

bool FinanceMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    ViewController * controller = nullptr;
    switch (selectedRow()) {
      case k_indexOfSimpleIntereset:
        if (!m_data->isSimpleInterest) {
          m_data->isSimpleInterest = true;
          m_data->m_data.m_simpleInterestData.resetValues();
        }
        controller = m_simpleInterestMenuController;
        break;
      case k_indexOfCompoundInterest:
        if (m_data->isSimpleInterest) {
          m_data->isSimpleInterest = false;
          m_data->m_data.m_compoundInterestData.resetValues();
        }
        controller = m_compoundInterestMenuController;
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
