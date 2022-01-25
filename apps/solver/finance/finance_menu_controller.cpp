#include "finance_menu_controller.h"
#include <apps/i18n.h>
#include <escher/container.h>
#include <assert.h>

using namespace Solver;

FinanceMenuController::FinanceMenuController(Escher::StackViewController * parentResponder, InterestMenuController * interestMenuController, FinanceData * financeData) :
      Shared::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfFinanceCells>(parentResponder),
      m_interestMenuController(interestMenuController),
      m_financeData(financeData) {
  selectRow(0);
  m_cells[k_indexOfSimpleInterest].setMessage(I18n::Message::SimpleInterest);
  m_cells[k_indexOfSimpleInterest].setSubtitle(I18n::Message::SimpleInterestDescription);
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
    bool simpleInterestRowSelected = (selectedRow() == k_indexOfSimpleInterest);
    assert(simpleInterestRowSelected || selectedRow() == k_indexOfCompoundInterest);

    // Update and reset fianceData's interest data model
    InterestData * interestData = m_financeData->interestData();
    InterestData::Initialize(interestData, simpleInterestRowSelected);
    m_interestMenuController->setData(interestData);

    stackOpenPage(m_interestMenuController, 0);
  } else if (event == Ion::Events::Left) {
    popStackViewControllerParentResponder();
  } else {
    return false;
  }
  return true;
}
