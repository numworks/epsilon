#include "edit_expression_controller.h"
#include "app.h"
#include "../apps_container.h"
#include <ion/display.h>
#include <poincare/preferences.h>
#include <assert.h>

using namespace Shared;

namespace Calculation {

EditExpressionController::ContentView::ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate, ScrollableExpressionViewWithCursorDelegate * scrollableExpressionViewWithCursorDelegate) :
  View(),
  m_mainView(subview),
  m_editableExpressionView(parentResponder, textFieldDelegate, scrollableExpressionViewWithCursorDelegate)
{
}

View * EditExpressionController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (index == 0) {
    return m_mainView;
  }
  assert(index == 1);
  return &m_editableExpressionView;
}

void EditExpressionController::ContentView::layoutSubviews() {
  KDCoordinate inputViewFrameHeight = m_editableExpressionView.minimalSizeForOptimalDisplay().height();
  KDRect mainViewFrame(0, 0, bounds().width(), bounds().height() - inputViewFrameHeight);
  m_mainView->setFrame(mainViewFrame);
  KDRect inputViewFrame(0, bounds().height() - inputViewFrameHeight, bounds().width(), inputViewFrameHeight);
  m_editableExpressionView.setFrame(inputViewFrame);
}

void EditExpressionController::ContentView::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

EditExpressionController::EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore) :
  DynamicViewController(parentResponder),
  m_historyController(historyController),
  m_calculationStore(calculationStore)
{
}

const char * EditExpressionController::textBody() {
  return ((ContentView *)view())->editableExpressionView()->text();
}

void EditExpressionController::insertTextBody(const char * text) {
  ((ContentView *)view())->editableExpressionView()->insertText(text);
}

bool EditExpressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (m_calculationStore->numberOfCalculations() > 0) {
       ((ContentView *)view())->editableExpressionView()->setEditing(false, false);
       app()->setFirstResponder(m_historyController);
    }
    return true;
  }
  return false;
}

void EditExpressionController::didBecomeFirstResponder() {
  int lastRow = m_calculationStore->numberOfCalculations() > 0 ? m_calculationStore->numberOfCalculations()-1 : 0;
  m_historyController->scrollToCell(0, lastRow);
  ((ContentView *)view())->editableExpressionView()->setEditing(true, false);
  app()->setFirstResponder(((ContentView *)view())->editableExpressionView());
}

bool EditExpressionController::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  assert(textField == ((ContentView *)view())->editableExpressionView()->textField());
  if (textField->isEditing() && textField->textFieldShouldFinishEditing(event) && textField->draftTextLength() == 0 && m_calculationStore->numberOfCalculations() > 0) {
    return inputViewDidReceiveEvent(event);
  }
  return textFieldDelegateApp()->textFieldDidReceiveEvent(textField, event);
}

bool EditExpressionController::textFieldDidFinishEditing(::TextField * textField, const char * text, Ion::Events::Event event) {
  assert(textField == ((ContentView *)view())->editableExpressionView()->textField());
  return inputViewDidFinishEditing(text, event);
}

bool EditExpressionController::textFieldDidAbortEditing(::TextField * textField, const char * text) {
  assert(textField == ((ContentView *)view())->editableExpressionView()->textField());
  return inputViewDidAbortEditing(text);
}

bool EditExpressionController::scrollableExpressionViewWithCursorDidReceiveEvent(::ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) {
  assert(scrollableExpressionViewWithCursor == ((ContentView *)view())->editableExpressionView()->scrollableExpressionViewWithCursor());
  if (scrollableExpressionViewWithCursor->isEditing() && scrollableExpressionViewWithCursor->scrollableExpressionViewWithCursorShouldFinishEditing(event) && !expressionLayout()->hasText() && m_calculationStore->numberOfCalculations() > 0) {
    return inputViewDidReceiveEvent(event);
  }
  return editableExpressionViewDelegateApp()->scrollableExpressionViewWithCursorDidReceiveEvent(scrollableExpressionViewWithCursor, event);
}

bool EditExpressionController::scrollableExpressionViewWithCursorDidFinishEditing(::ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, const char * text, Ion::Events::Event event) {
  assert(scrollableExpressionViewWithCursor == ((ContentView *)view())->editableExpressionView()->scrollableExpressionViewWithCursor());
  return inputViewDidFinishEditing(text, event);
}

bool EditExpressionController::scrollableExpressionViewWithCursorDidAbortEditing(::ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, const char * text) {
  assert(scrollableExpressionViewWithCursor == ((ContentView *)view())->editableExpressionView()->scrollableExpressionViewWithCursor());
  return inputViewDidAbortEditing(text);
}

void EditExpressionController::scrollableExpressionViewWithCursorDidChangeSize(::ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) {
  assert(scrollableExpressionViewWithCursor == ((ContentView *)view())->editableExpressionView()->scrollableExpressionViewWithCursor());
  reloadView();
}

TextFieldDelegateApp * EditExpressionController::textFieldDelegateApp() {
  return (App *)app();
}

EditableExpressionViewDelegateApp * EditExpressionController::editableExpressionViewDelegateApp() {
  return (App *)app();
}

View * EditExpressionController::loadView() {
  return new ContentView(this, (TableView *)m_historyController->view(), this, this);
}

void EditExpressionController::unloadView(View * view) {
  delete expressionLayout();
  delete view;
}

void EditExpressionController::reloadView() {
  ((ContentView *)view())->reload();
  m_historyController->reload();
  if (m_historyController->numberOfRows() > 0) {
    ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  }
}

bool EditExpressionController::inputViewDidReceiveEvent(Ion::Events::Event event) {
  App * calculationApp = (App *)app();
  const char * lastTextBody = m_calculationStore->calculationAtIndex(m_calculationStore->numberOfCalculations()-1)->inputText();
  m_calculationStore->push(lastTextBody, calculationApp->localContext());
  m_historyController->reload();
  ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  return true;
}

bool EditExpressionController::inputViewDidFinishEditing(const char * text, Ion::Events::Event event) {
  App * calculationApp = (App *)app();
  m_calculationStore->push(textBody(), calculationApp->localContext());
  m_historyController->reload();
  ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  ((ContentView *)view())->editableExpressionView()->setEditing(true, true);
  ((ContentView *)view())->editableExpressionView()->setText("");
  return true;
}

bool EditExpressionController::inputViewDidAbortEditing(const char * text) {
  ((ContentView *)view())->editableExpressionView()->setEditing(true, true);
  ((ContentView *)view())->editableExpressionView()->setText(text);
  return false;
}

void EditExpressionController::viewDidDisappear() {
  DynamicViewController::viewDidDisappear();
  m_historyController->viewDidDisappear();
}

Poincare::ExpressionLayout * EditExpressionController::expressionLayout() {
  return ((ContentView *)view())->editableExpressionView()->scrollableExpressionViewWithCursor()->expressionViewWithCursor()->expressionView()->expressionLayout();
}

}
