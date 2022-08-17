#include "menu_controller.h"
#include <apps/i18n.h>
#include <escher/container.h>
#include <assert.h>

using namespace Finance;

MenuController::MenuController(Escher::StackViewController * parentResponder, InterestMenuController * interestMenuController, InterestData * interestData) :
      Escher::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfFinanceCells, Escher::RegularListViewDataSource>(parentResponder),
      m_interestMenuController(interestMenuController),
      m_interestData(interestData) {
  selectRow(0);
  cellAtIndex(k_indexOfSimpleInterest)->setMessage(I18n::Message::SimpleInterest);
  cellAtIndex(k_indexOfSimpleInterest)->setSubtitle(I18n::Message::SimpleInterestDescription);
  cellAtIndex(k_indexOfCompoundInterest)->setMessage(I18n::Message::CompoundInterest);
  cellAtIndex(k_indexOfCompoundInterest)->setSubtitle(I18n::Message::CompoundInterestDescription);
}

void MenuController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    bool simpleInterestRowSelected = (selectedRow() == k_indexOfSimpleInterest);
    assert(simpleInterestRowSelected || selectedRow() == k_indexOfCompoundInterest);

    // Initialize a new interest data model
    InterestData::Initialize(m_interestData, simpleInterestRowSelected);
    m_interestMenuController->selectRow(0);
    stackOpenPage(m_interestMenuController);
    return true;
  }
  return false;
}
