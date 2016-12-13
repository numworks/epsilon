#include "edit_expression_controller.h"
#include "app.h"
#include <assert.h>

namespace Calculation {

EditExpressionController::ContentView::ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate) :
  View(),
  m_mainView(subview),
  m_textField(parentResponder, m_textBody, 255, textFieldDelegate)
{
  m_textBody[0] = 0;
}

int EditExpressionController::ContentView::numberOfSubviews() const {
  return 2;
}

View * EditExpressionController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return m_mainView;
    case 1:
      return &m_textField;
    default:
      assert(false);
      return nullptr;
  }
}

void EditExpressionController::ContentView::layoutSubviews() {
  KDRect mainViewFrame(0, 0, bounds().width(), bounds().height() - k_textFieldHeight);
  m_mainView->setFrame(mainViewFrame);
  KDRect inputViewFram(0, bounds().height() - k_textFieldHeight, bounds().width(), k_textFieldHeight);
  m_textField.setFrame(inputViewFram);
}

TextField * EditExpressionController::ContentView::textField() {
  return &m_textField;
}

TableView * EditExpressionController::ContentView::mainView() {
  return m_mainView;
}

EditExpressionController::EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore) :
  ViewController(parentResponder),
  m_contentView(this, (TableView *)historyController->view(), this),
  m_historyController(historyController),
  m_calculationStore(calculationStore)
{
}

View * EditExpressionController::view() {
  return &m_contentView;
}

const char * EditExpressionController::title() const {
  return "EditExpressionController";
}

const char * EditExpressionController::textBody() {
  return m_contentView.textField()->text();
}

void EditExpressionController::setTextBody(const char * text) {
  m_contentView.textField()->setText(text);
}

bool EditExpressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (m_calculationStore->numberOfCalculations() > 0) {
      app()->setFirstResponder(m_historyController);
    }
    return true;
  }
  return false;
}

void EditExpressionController::didBecomeFirstResponder() {
  app()->setFirstResponder(m_contentView.textField());
}

bool EditExpressionController::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  App * myApp = (App *)app();
  return myApp->textFieldDidReceiveEvent(textField, event);
}

bool EditExpressionController::textFieldDidFinishEditing(::TextField * textField, const char * text) {
  Calculation calculation = Calculation();
  App * calculationApp = (App *)app();
  calculation.setContent(textBody(), calculationApp->evaluateContext());
  m_calculationStore->push(&calculation);
  m_historyController->reload();
  m_contentView.mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  m_contentView.textField()->setText("");
  return true;
}

}
