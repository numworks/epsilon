#include "finance_menu_controller.h"
#include <apps/i18n.h>
#include <escher/container.h>
#include <assert.h>

using namespace Solver;

FinanceMenuController::FinanceMenuController(Escher::StackViewController * parentResponder, InterestMenuController * interestMenuController, InterestData * interestData) :
      Escher::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfFinanceCells>(parentResponder),
      m_interestMenuController(interestMenuController),
      m_interestData(interestData) {
  selectRow(0);
  cellAtIndex(k_indexOfSimpleInterest)->setMessage(I18n::Message::SimpleInterest);
  cellAtIndex(k_indexOfSimpleInterest)->setSubtitle(I18n::Message::SimpleInterestDescription);
  cellAtIndex(k_indexOfCompoundInterest)->setMessage(I18n::Message::CompoundInterest);
  cellAtIndex(k_indexOfCompoundInterest)->setSubtitle(I18n::Message::CompoundInterestDescription);
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

    // Initialize a new interest data model
    InterestData::Initialize(m_interestData, simpleInterestRowSelected);
    m_interestMenuController->selectRow(0);
    stackOpenPage(m_interestMenuController);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}
