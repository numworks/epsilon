#include "function_list_controller.h"

#include <apps/i18n.h>

#include "function_app.h"

using namespace Escher;

namespace Shared {

FunctionListController::FunctionListController(Responder* parentResponder,
                                               ButtonRowController* header,
                                               ButtonRowController* footer,
                                               I18n::Message text)
    : ExpressionModelListController(parentResponder, text),
      ButtonRowDelegate(header, footer),
      m_parameterColumnSelected(false),
      m_selectableListView(this, this, this, this),
      m_plotButton(this, I18n::Message::Plot,
                   Invocation::Builder<FunctionListController>(
                       [](FunctionListController* list, void* sender) {
                         TabViewController* tabController =
                             list->tabController();
                         tabController->setActiveTab(1);
                         return true;
                       },
                       this),
                   ButtonCell::Style::EmbossedGray, KDFont::Size::Small,
                   Palette::PurpleBright),
      m_valuesButton(this, I18n::Message::DisplayValues,
                     Invocation::Builder<FunctionListController>(
                         [](FunctionListController* list, void* sender) {
                           TabViewController* tabController =
                               list->tabController();
                           tabController->setActiveTab(2);
                           return true;
                         },
                         this),
                     ButtonCell::Style::EmbossedGray, KDFont::Size::Small,
                     Palette::PurpleBright) {
  m_selectableListView.resetMargins();
  m_selectableListView.setVerticalCellOverlap(0);
}

/* ButtonRowDelegate */

int FunctionListController::numberOfButtons(
    ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Bottom) {
    return 2;
  }
  return 0;
}

ButtonCell* FunctionListController::buttonAtIndex(
    int index, ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Top) {
    return nullptr;
  }
  const ButtonCell* buttons[2] = {&m_plotButton, &m_valuesButton};
  return (ButtonCell*)buttons[index];
}

/* Responder */

void FunctionListController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  switch (event.type) {
    case ResponderChainEventType::DidEnter: {
      selectableListView()->reloadData(false);
      break;
    }
    case ResponderChainEventType::WillExit: {
      if (event.nextFirstResponder == tabController()) {
        assert(tabController() != nullptr);
        selectableListView()->deselectTable();
        footer()->setSelectedButton(-1);
      }
      break;
    }
    case ResponderChainEventType::HasBecomeFirst: {
      if (selectedRow() == -1) {
        selectRow(0);
      } else {
        selectRow(selectedRow());
      }
      if (selectedRow() >= numberOfRows()) {
        selectRow(numberOfRows() - 1);
      }
      footer()->setSelectedButton(-1);
      if (m_editedCellIndex != -1) {
        // Resume edition if it was interrupted by a store
        App::app()->setFirstResponder(layoutField());
      } else {
        App::app()->setFirstResponder(selectableListView());
      }
    }
    default: {
      ExpressionModelListController::handleResponderChainEvent(event);
      break;
    }
  }
}

bool FunctionListController::handleEvent(Ion::Events::Event event) {
  if (selectedRow() >= 0 && selectedRow() <= numberOfRows() &&
      !isAddEmptyRow(selectedRow())) {
    if (m_parameterColumnSelected) {
      if ((event == Ion::Events::Left && parameterColumnPosition().isRight()) ||
          (event == Ion::Events::Right && parameterColumnPosition().isLeft())) {
        // Leave parameter column
        m_parameterColumnSelected = false;
        selectableListView()->reloadData(true, false);
        return true;
      }
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        // Open function parameter menu
        StackViewController* stack = stackController();
        parameterController()->setRecord(selectedRecord());
        stack->push(parameterController());
        return true;
      }
      if (event != Ion::Events::Backspace && event != Ion::Events::Up) {
        // Handled later
        return false;
      }
    }
    if (!m_parameterColumnSelected &&
        ((event == Ion::Events::Right && parameterColumnPosition().isRight()) ||
         (event == Ion::Events::Left && parameterColumnPosition().isLeft()))) {
      // Enter parameter column
      m_parameterColumnSelected = true;
      selectableListView()->reloadData(true, false);
      return true;
    }
  }
  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      footer()->setSelectedButton(-1);
      selectableListView()->selectCell(numberOfRows() - 1);
      App::app()->setFirstResponder(selectableListView());
      return true;
    }
    selectableListView()->deselectTable();
    assert(selectedRow() == -1);
    tabController()->selectTab();
    return true;
  }
  if (selectedRow() < 0) {
    return false;
  }
  if (event == Ion::Events::Down) {
    selectableListView()->deselectTable();
    footer()->setSelectedButton(0);
    return true;
  }
  return handleEventOnExpression(event);
}

/* ExpressionModelListController */

void FunctionListController::fillCellForRow(HighlightCell* cell, int row) {
  EvenOddCell* evenOddCell = static_cast<EvenOddCell*>(cell);
  evenOddCell->setEven(modelIndexForRow(row) % 2 == 0);
  evenOddCell->reloadCell();
}

StackViewController* FunctionListController::stackController() const {
  return static_cast<StackViewController*>(
      parentResponder()->parentResponder()->parentResponder());
}

TabViewController* FunctionListController::tabController() const {
  return static_cast<TabViewController*>(parentResponder()
                                             ->parentResponder()
                                             ->parentResponder()
                                             ->parentResponder());
}

}  // namespace Shared
