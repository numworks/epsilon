#include <apps/i18n.h>
#include "function_list_controller.h"
#include "function_app.h"

using namespace Escher;

namespace Shared {

FunctionListController::FunctionListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer, I18n::Message text) :
  ExpressionModelListController(parentResponder, text),
  ButtonRowDelegate(header, footer),
  m_plotButton(this, I18n::Message::Plot, Invocation([](void * context, void * sender) {
      FunctionListController * list = (FunctionListController *)context;
      TabViewController * tabController = list->tabController();
      tabController->setActiveTab(1);
      return true;
    }, this), KDFont::Size::Small, Palette::PurpleBright),
  m_valuesButton(this, I18n::Message::DisplayValues, Invocation([](void * context, void * sender) {
      FunctionListController * list = (FunctionListController *)context;
      TabViewController * tabController = list->tabController();
      tabController->setActiveTab(2);
      return true;
    }, this), KDFont::Size::Small, Palette::PurpleBright)
{}

/* ButtonRowDelegate */

int FunctionListController::numberOfButtons(ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Bottom) {
    return 2;
  }
  return 0;
}

AbstractButtonCell * FunctionListController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Top) {
    return nullptr;
  }
  const AbstractButtonCell * buttons[2] = {&m_plotButton, &m_valuesButton};
  return (AbstractButtonCell *)buttons[index];
}

/* Responder */

void FunctionListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectableTableView()->reloadData();
}

void FunctionListController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    selectableTableView()->deselectTable();
    footer()->setSelectedButton(-1);
  }
}

/* ExpressionModelListController */

StackViewController * FunctionListController::stackController() const {
  return static_cast<StackViewController *>(parentResponder()->parentResponder()->parentResponder());
}

void FunctionListController::configureFunction(Ion::Storage::Record record) {
  StackViewController * stack = stackController();
  parameterController()->setRecord(record);
  stack->push(parameterController());
}

TabViewController * FunctionListController::tabController() const {
  return static_cast<TabViewController *>(parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

InputViewController * FunctionListController::inputController() {
  return FunctionApp::app()->inputViewController();
}

}
