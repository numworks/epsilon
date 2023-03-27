#include "menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

#include "app.h"

using namespace Finance;

MenuController::MenuController(Escher::StackViewController* parentResponder,
                               InterestMenuController* interestMenuController)
    : Escher::SelectableCellListPage<
          Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                           Escher::ChevronView>,
          k_numberOfFinanceCells, Escher::RegularListViewDataSource>(
          parentResponder),
      m_interestMenuController(interestMenuController) {
  selectRow(0);
  cellAtIndex(k_indexOfSimpleInterest)
      ->label()
      ->setMessage(I18n::Message::SimpleInterest);
  cellAtIndex(k_indexOfSimpleInterest)
      ->subLabel()
      ->setMessage(I18n::Message::SimpleInterestDescription);
  cellAtIndex(k_indexOfCompoundInterest)
      ->label()
      ->setMessage(I18n::Message::CompoundInterest);
  cellAtIndex(k_indexOfCompoundInterest)
      ->subLabel()
      ->setMessage(I18n::Message::CompoundInterestDescription);
  centerTable(Escher::Metric::DisplayHeightWithoutTitleBar);
}

void MenuController::didBecomeFirstResponder() {
  m_selectableListView.reloadData();
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (cellAtIndex(0)->canBeActivatedByEvent(event)) {
    bool simpleInterestRowSelected = (selectedRow() == k_indexOfSimpleInterest);
    assert(simpleInterestRowSelected ||
           selectedRow() == k_indexOfCompoundInterest);

    // Set the interest data model
    App::SetModel(simpleInterestRowSelected);
    m_interestMenuController->selectRow(0);
    stackOpenPage(m_interestMenuController);
    return true;
  }
  return false;
}
