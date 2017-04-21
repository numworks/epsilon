#include "edit_expression_controller.h"
#include "../apps_container.h"
#include "app.h"
#include <assert.h>

using namespace Shared;

namespace Calculation {

EditExpressionController::ContentView::ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate) :
  View(),
  m_mainView(subview),
  m_textField(parentResponder, m_textBody, 100, textFieldDelegate)
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
  KDRect inputViewFrame(0, bounds().height() - k_textFieldHeight, bounds().width(), k_textFieldHeight);
  m_textField.setFrame(inputViewFrame);
}

void  EditExpressionController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() -k_textFieldHeight-k_separatorThickness, bounds().width(), k_separatorThickness), Palette::GreyMiddle);
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

void EditExpressionController::setTextBody(const char * text) {
  ((ContentView *)view())->textField()->setEditing(true);
  ((ContentView *)view())->textField()->setText(text);
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

void EditExpressionController::loadView() {
  m_historyController->loadView();
  DynamicViewController::loadView();
}

void EditExpressionController::unloadView() {
  m_historyController->unloadView();
  DynamicViewController::unloadView();
}

View * EditExpressionController::createView() {
  return new ContentView(this, (TableView *)m_historyController->view(), this);
}

}
