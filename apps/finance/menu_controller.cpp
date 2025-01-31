#include "menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

#include "app.h"

using namespace Finance;

MenuController::MenuController(Escher::StackViewController* parentResponder,
                               InterestMenuController* interestMenuController)
    : Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                           Escher::ChevronView>,
          k_numberOfCells>(parentResponder),
      m_interestMenuController(interestMenuController) {
  selectRow(0);
  cell(k_indexOfSimpleInterest)
      ->label()
      ->setMessage(I18n::Message::SimpleInterest);
  cell(k_indexOfSimpleInterest)
      ->subLabel()
      ->setMessage(I18n::Message::SimpleInterestDescription);
  cell(k_indexOfCompoundInterest)
      ->label()
      ->setMessage(I18n::Message::CompoundInterest);
  cell(k_indexOfCompoundInterest)
      ->subLabel()
      ->setMessage(I18n::Message::CompoundInterestDescription);
  centerTable(Escher::Metric::DisplayHeightWithoutTitleBar);
}

void MenuController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::BecameFirst) {
    m_selectableListView.reloadData();
  } else {
    Escher::UniformSelectableListController<
        Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                         Escher::ChevronView>,
        k_numberOfCells>::handleResponderChainEvent(event);
  }
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (cell(0)->canBeActivatedByEvent(event)) {
    bool simpleInterestRowSelected = (selectedRow() == k_indexOfSimpleInterest);
    assert(simpleInterestRowSelected ||
           selectedRow() == k_indexOfCompoundInterest);

    // Set the interest data model
    App::SetModel(simpleInterestRowSelected);
    m_interestMenuController->selectFirstCell();
    stackOpenPage(m_interestMenuController);
    return true;
  }
  return false;
}
