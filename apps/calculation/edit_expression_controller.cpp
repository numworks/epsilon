#include "edit_expression_controller.h"
#include "app.h"
#include <assert.h>

namespace Calculation {

EditExpressionController::ContentView::ContentView(TableView * subview, TextFieldDelegate * textFieldDelegate) :
  View(),
  m_mainView(subview),
  m_textField(nullptr, m_textBody, 255, textFieldDelegate)
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

EditExpressionController::EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore, TextFieldDelegate * textFieldDelegate) :
  ViewController(parentResponder),
  m_contentView((TableView *)historyController->view(), textFieldDelegate),
  m_historyController(historyController),
  m_calculationStore(calculationStore)
{
  m_contentView.textField()->setParentResponder(this);
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
  switch (event) {
    case Ion::Events::Event::ENTER:
    {
      Calculation calculation = Calculation();
      App * calculationApp = (App *)app();
      calculation.setContent(textBody(), calculationApp->evaluateContext());
      m_calculationStore->push(&calculation);
      m_historyController->reload();
      m_contentView.mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
      m_contentView.textField()->setText("");
      return true;
    }
    case Ion::Events::Event::ESC:
      return true;
    case Ion::Events::Event::UP_ARROW:
      if (m_calculationStore->numberOfCalculations() > 0) {
        app()->setFirstResponder(m_historyController);
      }
      return true;
    default:
      return false;
  }
}

void EditExpressionController::didBecomeFirstResponder() {
  app()->setFirstResponder(m_contentView.textField());
}

void EditExpressionController::edit(const char * initialContent) {
  setTextBody(initialContent);
  app()->setFirstResponder(this);
}

}
