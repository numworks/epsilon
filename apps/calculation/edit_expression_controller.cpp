#include "edit_expression_controller.h"
#include "../apps_container.h"
#include "app.h"
#include <assert.h>

using namespace Shared;

namespace Calculation {

EditExpressionController::ContentView::ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate) :
  View(),
  m_mainView(subview),
  m_textField(parentResponder, m_textBody, TextField::maxBufferSize(), textFieldDelegate)
{
  m_textBody[0] = 0;
}

int EditExpressionController::ContentView::numberOfSubviews() const {
  return 2;
}

View * EditExpressionController::ContentView::subviewAtIndex(int index) {
  View * views[2] = {m_mainView, &m_textField};
  return views[index];
}

void EditExpressionController::ContentView::layoutSubviews() {
  KDRect mainViewFrame(0, 0, bounds().width(), bounds().height() - k_textFieldHeight-k_separatorThickness);
  m_mainView->setFrame(mainViewFrame);
  KDRect inputViewFrame(k_textMargin, bounds().height() - k_textFieldHeight, bounds().width()-k_textMargin, k_textFieldHeight);
  m_textField.setFrame(inputViewFrame);
}

void  EditExpressionController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw the separator
  ctx->fillRect(KDRect(0, bounds().height() -k_textFieldHeight-k_separatorThickness, bounds().width(), k_separatorThickness), Palette::GreyMiddle);
  // Color the margin
  ctx->fillRect(KDRect(0, bounds().height() -k_textFieldHeight, k_textMargin, k_textFieldHeight), m_textField.backgroundColor());
}

TextField * EditExpressionController::ContentView::textField() {
  return &m_textField;
}

TableView * EditExpressionController::ContentView::mainView() {
  return m_mainView;
}

EditExpressionController::EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore) :
  DynamicViewController(parentResponder),
  m_historyController(historyController),
  m_calculationStore(calculationStore)
{
}

const char * EditExpressionController::textBody() {
  return ((ContentView *)view())->textField()->text();
}

void EditExpressionController::insertTextBody(const char * text) {
  TextField * tf = ((ContentView *)view())->textField();
  tf->setEditing(true, false);
  tf->insertTextAtLocation(text, tf->cursorLocation());
  tf->setCursorLocation(tf->cursorLocation() + strlen(text));
}

bool EditExpressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (m_calculationStore->numberOfCalculations() > 0) {
      ((ContentView *)view())->textField()->setEditing(false, false);
      app()->setFirstResponder(m_historyController);
    }
    return true;
  }
  return false;
}

void EditExpressionController::didBecomeFirstResponder() {
  int lastRow = m_calculationStore->numberOfCalculations() > 0 ? m_calculationStore->numberOfCalculations()-1 : 0;
  m_historyController->scrollToCell(0, lastRow);
  ((ContentView *)view())->textField()->setEditing(true, false);
  app()->setFirstResponder(((ContentView *)view())->textField());
}

bool EditExpressionController::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  if (textField->textFieldShouldFinishEditing(event) && textField->isEditing() && strlen(textField->text()) == 0 && m_calculationStore->numberOfCalculations() > 0) {
    App * calculationApp = (App *)app();
    const char * lastTextBody = m_calculationStore->calculationAtIndex(m_calculationStore->numberOfCalculations()-1)->inputText();
    m_calculationStore->push(lastTextBody, calculationApp->localContext());
    m_historyController->reload();
    ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
    return true;
  }
  return textFieldDelegateApp()->textFieldDidReceiveEvent(textField, event);
}

bool EditExpressionController::textFieldDidFinishEditing(::TextField * textField, const char * text, Ion::Events::Event event) {
  App * calculationApp = (App *)app();
  m_calculationStore->push(textBody(), calculationApp->localContext());
  m_historyController->reload();
  ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  ((ContentView *)view())->textField()->setEditing(true);
  ((ContentView *)view())->textField()->setText("");
  return true;
}

bool EditExpressionController::textFieldDidAbortEditing(::TextField * textField, const char * text) {
  ((ContentView *)view())->textField()->setEditing(true);
  ((ContentView *)view())->textField()->setText(text);
  return false;
}

TextFieldDelegateApp * EditExpressionController::textFieldDelegateApp() {
  return (App *)app();
}

View * EditExpressionController::loadView() {
  return new ContentView(this, (TableView *)m_historyController->view(), this);
}

void EditExpressionController::unloadView(View * view) {
  delete view;
}

void EditExpressionController::viewDidDisappear() {
  DynamicViewController::viewDidDisappear();
  m_historyController->viewDidDisappear();
}

}
