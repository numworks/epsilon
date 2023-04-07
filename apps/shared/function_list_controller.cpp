#include "function_list_controller.h"

#include <apps/i18n.h>

#include "function_app.h"

using namespace Escher;

namespace Shared {

FunctionListController::FunctionListController(Responder *parentResponder,
                                               ButtonRowController *header,
                                               ButtonRowController *footer,
                                               I18n::Message text)
    : ExpressionModelListController(parentResponder, text),
      ButtonRowDelegate(header, footer),
      InputEventHandlerDelegate(),
      m_selectableListView(this, this, this, this),
      m_plotButton(this, I18n::Message::Plot,
                   Invocation::Builder<FunctionListController>(
                       [](FunctionListController *list, void *sender) {
                         TabViewController *tabController =
                             list->tabController();
                         tabController->setActiveTab(1);
                         return true;
                       },
                       this),
                   KDFont::Size::Small, Palette::PurpleBright),
      m_valuesButton(this, I18n::Message::DisplayValues,
                     Invocation::Builder<FunctionListController>(
                         [](FunctionListController *list, void *sender) {
                           TabViewController *tabController =
                               list->tabController();
                           tabController->setActiveTab(2);
                           return true;
                         },
                         this),
                     KDFont::Size::Small, Palette::PurpleBright) {
  m_selectableListView.setMargins(0);
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

AbstractButtonCell *FunctionListController::buttonAtIndex(
    int index, ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Top) {
    return nullptr;
  }
  const AbstractButtonCell *buttons[2] = {&m_plotButton, &m_valuesButton};
  return (AbstractButtonCell *)buttons[index];
}

/* Responder */

void FunctionListController::didEnterResponderChain(
    Responder *previousFirstResponder) {
  selectableListView()->reloadData(false);
}

void FunctionListController::willExitResponderChain(
    Responder *nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    selectableListView()->deselectTable();
    footer()->setSelectedButton(-1);
  }
}

void FunctionListController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCell(0);
  } else {
    selectCell(selectedRow());
  }
  if (selectedRow() >= numberOfExpressionRows()) {
    selectCell(numberOfExpressionRows() - 1);
  }
  footer()->setSelectedButton(-1);
  if (m_editedCellIndex != -1) {
    // Resume edition if it was interrupted by a store
    Container::activeApp()->setFirstResponder(expressionField());
  } else {
    Container::activeApp()->setFirstResponder(selectableListView());
  }
}

/* ExpressionModelListController */

StackViewController *FunctionListController::stackController() const {
  return static_cast<StackViewController *>(
      parentResponder()->parentResponder()->parentResponder());
}

void FunctionListController::configureFunction(Ion::Storage::Record record) {
  StackViewController *stack = stackController();
  parameterController()->setRecord(record);
  stack->push(parameterController());
}

TabViewController *FunctionListController::tabController() const {
  return static_cast<TabViewController *>(parentResponder()
                                              ->parentResponder()
                                              ->parentResponder()
                                              ->parentResponder());
}

}  // namespace Shared
