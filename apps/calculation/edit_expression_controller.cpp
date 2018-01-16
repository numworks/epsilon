#include "edit_expression_controller.h"
#include "../apps_container.h"
#include "ion/display.h"
#include "app.h"
#include <assert.h>

using namespace Shared;

namespace Calculation {

EditExpressionController::ContentView::ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate, EditableExpressionViewDelegate * editableExpressionViewDelegate) :
  View(),
  m_mainView(subview),
  m_textField(parentResponder, m_textBody, TextField::maxBufferSize(), textFieldDelegate),
  m_editableExpressionView(parentResponder, new Poincare::HorizontalLayout(), editableExpressionViewDelegate)
{
  m_textBody[0] = 0;
}

int EditExpressionController::ContentView::numberOfSubviews() const {
  return 2;
}

View * EditExpressionController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (index == 0) {
    return m_mainView;
  }
  assert(index == 1);
  if (editionIsInTextField()) {
    return &m_textField;
  }
  return &m_editableExpressionView;
}

void EditExpressionController::ContentView::layoutSubviews() {
  KDCoordinate inputViewFrameHeight = inputViewHeight();
  KDRect mainViewFrame(0, 0, bounds().width(), bounds().height() - inputViewFrameHeight-k_separatorThickness);
  m_mainView->setFrame(mainViewFrame);
  if (editionIsInTextField()) {
    KDRect inputViewFrame(k_leftMargin, bounds().height() - inputViewFrameHeight + k_verticalMargin, bounds().width()-k_leftMargin, k_textFieldHeight - k_verticalMargin);
    m_textField.setFrame(inputViewFrame);
    m_editableExpressionView.setFrame(KDRectZero);
    return;
  }
  KDRect inputViewFrame(k_leftMargin, bounds().height() - inputViewFrameHeight, bounds().width() - k_leftMargin, inputViewFrameHeight);
  m_editableExpressionView.setFrame(inputViewFrame);
  m_textField.setFrame(KDRectZero);
}

void EditExpressionController::ContentView::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

void EditExpressionController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate inputViewFrameHeight = inputViewHeight();
  // Draw the separator
  ctx->fillRect(KDRect(0, bounds().height() -inputViewFrameHeight-k_separatorThickness, bounds().width(), k_separatorThickness), Palette::GreyMiddle);
  // Color the left margin
  ctx->fillRect(KDRect(0, bounds().height() -inputViewFrameHeight, k_leftMargin, inputViewFrameHeight), m_textField.backgroundColor());
  if (!editionIsInTextField()) {
    // Color the upper margin
    ctx->fillRect(KDRect(0, bounds().height() -inputViewFrameHeight, bounds().width(), k_verticalMargin), m_textField.backgroundColor());
  }
}

KDCoordinate EditExpressionController::ContentView::inputViewHeight() const {
  return  k_verticalMargin + (editionIsInTextField() ? k_textFieldHeight : editableExpressionViewHeight());
}

KDCoordinate EditExpressionController::ContentView::editableExpressionViewHeight() const {
  return KDCoordinate(min(0.6*Ion::Display::Height, max(k_textFieldHeight, m_editableExpressionView.minimalSizeForOptimalDisplay().height()+k_verticalMargin)));
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
  if (((ContentView *)view())->editionIsInTextField()) {
    TextField * tf = ((ContentView *)view())->textField();
    tf->setEditing(true, false);
    tf->insertTextAtLocation(text, tf->cursorLocation());
    tf->setCursorLocation(tf->cursorLocation() + strlen(text));
    return;
  }
  EditableExpressionView * editableExpressionView = ((ContentView *)view())->editableExpressionView();
  editableExpressionView->setEditing(true);
  editableExpressionView->insertLayoutFromTextAtCursor(text);
}

bool EditExpressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (m_calculationStore->numberOfCalculations() > 0) {
      if (((ContentView *)view())->editionIsInTextField()) {
        ((ContentView *)view())->textField()->setEditing(false, false);
      } else {
        ((ContentView *)view())->editableExpressionView()->setEditing(false);
      }
      app()->setFirstResponder(m_historyController);
    }
    return true;
  }
  return false;
}

void EditExpressionController::didBecomeFirstResponder() {
  int lastRow = m_calculationStore->numberOfCalculations() > 0 ? m_calculationStore->numberOfCalculations()-1 : 0;
  m_historyController->scrollToCell(0, lastRow);
  if (((ContentView *)view())->editionIsInTextField()) {
    ((ContentView *)view())->textField()->setEditing(true, false);
    app()->setFirstResponder(((ContentView *)view())->textField());
    return;
  }
  ((ContentView *)view())->editableExpressionView()->setEditing(true);
  app()->setFirstResponder(((ContentView *)view())->editableExpressionView());
}

bool EditExpressionController::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->textFieldShouldFinishEditing(event) && textField->draftTextLength() == 0 && m_calculationStore->numberOfCalculations() > 0) {
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

bool EditExpressionController::editableExpressionViewDidReceiveEvent(::EditableExpressionView * editableExpressionView, Ion::Events::Event event) {
  bool layoutIsEmpty = expressionLayout()->isHorizontal() && expressionLayout()->numberOfChildren() == 0;
  if (editableExpressionView->isEditing() && editableExpressionView->editableExpressionViewShouldFinishEditing(event) && layoutIsEmpty && m_calculationStore->numberOfCalculations() > 0) {
    App * calculationApp = (App *)app();
    const char * lastTextBody = m_calculationStore->calculationAtIndex(m_calculationStore->numberOfCalculations()-1)->inputText();
    m_calculationStore->push(lastTextBody, calculationApp->localContext());
    m_historyController->reload();
    ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
    return true;
  }
  return textFieldAndEditableExpressionViewDelegateApp()->editableExpressionViewDidReceiveEvent(editableExpressionView, event);
}

bool EditExpressionController::editableExpressionViewDidFinishEditing(::EditableExpressionView * editableExpressionView, const char * text, Ion::Events::Event event) {
  App * calculationApp = (App *)app();
  expressionLayout()->writeTextInBuffer(const_cast<char *>(textBody()), ContentView::k_bufferLength);
  m_calculationStore->push(textBody(), calculationApp->localContext());
  (const_cast<ExpressionView *>(((ContentView *)view())->editableExpressionView()->expressionViewWithCursor()->expressionView()))->setExpressionLayout(new Poincare::HorizontalLayout());
  reloadView();
  ((ContentView *)view())->editableExpressionView()->setEditing(true);
  return true;
}

bool EditExpressionController::editableExpressionViewDidAbortEditing(::EditableExpressionView * editableExpressionView, const char * text) {
  ((ContentView *)view())->editableExpressionView()->setEditing(true);
  //TODO ((ContentView *)view())->editableExpressionView()->editableExpressionView()->expressionViewWithCursor()->expressionView()->setLayout(;
  return false;
}

void EditExpressionController::editableExpressionViewDidChangeSize(::EditableExpressionView * editableExpressionView) {
  assert(editableExpressionView == ((ContentView *)view())->editableExpressionView());
  reloadView();
}

TextFieldDelegateApp * EditExpressionController::textFieldDelegateApp() {
  return (App *)app();
}

TextFieldAndEditableExpressionViewDelegateApp * EditExpressionController::textFieldAndEditableExpressionViewDelegateApp() {
  return (App *)app();
}

View * EditExpressionController::loadView() {
  return new ContentView(this, (TableView *)m_historyController->view(), this, this);
}

void EditExpressionController::unloadView(View * view) {
  delete view;
}

void EditExpressionController::reloadView() {
  ((ContentView *)view())->reload();
  m_historyController->reload();
  if (m_historyController->numberOfRows() > 0) {
    ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  }
}

void EditExpressionController::viewDidDisappear() {
  DynamicViewController::viewDidDisappear();
  m_historyController->viewDidDisappear();
}

Poincare::ExpressionLayout * EditExpressionController::expressionLayout() {
  return ((ContentView *)view())->editableExpressionView()->expressionViewWithCursor()->expressionView()->expressionLayout();
}

}
