#include "edit_expression_controller.h"
#include "app.h"
#include "../apps_container.h"
#include <ion/display.h>
#include <poincare/preferences.h>
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Calculation {

View * EditExpressionController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (index == 0) {
    return m_mainView;
  }
  assert(index == 1);
  return &m_expressionField;
}

void EditExpressionController::ContentView::layoutSubviews() {
  KDCoordinate inputViewFrameHeight = m_expressionField.minimalSizeForOptimalDisplay().height();
  KDRect mainViewFrame(0, 0, bounds().width(), bounds().height() - inputViewFrameHeight);
  m_mainView->setFrame(mainViewFrame);
  KDRect inputViewFrame(0, bounds().height() - inputViewFrameHeight, bounds().width(), inputViewFrameHeight);
  m_expressionField.setFrame(inputViewFrame);
}

void EditExpressionController::ContentView::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

EditExpressionController::EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore) :
  DynamicViewController(parentResponder),
  m_historyController(historyController),
  m_calculationStore(calculationStore),
  m_inputViewHeightIsMaximal(false)
{
  m_cacheBuffer[0] = 0;
}

void EditExpressionController::insertTextBody(const char * text) {
  ((ContentView *)view())->expressionField()->handleEventWithText(text, false, true);
}

bool EditExpressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (m_calculationStore->numberOfCalculations() > 0) {
      m_cacheBuffer[0] = 0;
      ((ContentView *)view())->expressionField()->setEditing(false, false);
      app()->setFirstResponder(m_historyController);
    }
    return true;
  }
  return false;
}

void EditExpressionController::didBecomeFirstResponder() {
  int lastRow = m_calculationStore->numberOfCalculations() > 0 ? m_calculationStore->numberOfCalculations()-1 : 0;
  m_historyController->scrollToCell(0, lastRow);
  ((ContentView *)view())->expressionField()->setEditing(true, false);
  app()->setFirstResponder(((ContentView *)view())->expressionField());
}

bool EditExpressionController::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->textFieldShouldFinishEditing(event) && textField->draftTextLength() == 0 && m_cacheBuffer[0] != 0) {
    return inputViewDidReceiveEvent(event);
  }
  return textFieldDelegateApp()->textFieldDidReceiveEvent(textField, event);
}

bool EditExpressionController::textFieldDidFinishEditing(::TextField * textField, const char * text, Ion::Events::Event event) {
  return inputViewDidFinishEditing(text, nullptr);
}

bool EditExpressionController::textFieldDidAbortEditing(::TextField * textField) {
  return inputViewDidAbortEditing(textField->text());
}

bool EditExpressionController::expressionLayoutFieldDidReceiveEvent(::ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  if (expressionLayoutField->isEditing() && expressionLayoutField->expressionLayoutFieldShouldFinishEditing(event) && !expressionLayoutField->hasText() && m_calculationStore->numberOfCalculations() > 0) {
    return inputViewDidReceiveEvent(event);
  }
  return expressionFieldDelegateApp()->expressionLayoutFieldDidReceiveEvent(expressionLayoutField, event);
}

bool EditExpressionController::expressionLayoutFieldDidFinishEditing(::ExpressionLayoutField * expressionLayoutField, ExpressionLayout * layout, Ion::Events::Event event) {
  return inputViewDidFinishEditing(nullptr, layout);
}

bool EditExpressionController::expressionLayoutFieldDidAbortEditing(::ExpressionLayoutField * expressionLayoutField) {
  return inputViewDidAbortEditing(nullptr);
}

void EditExpressionController::expressionLayoutFieldDidChangeSize(::ExpressionLayoutField * expressionLayoutField) {
  /* Reload the view only if the ExpressionField height actually changes, i.e.
   * not if the height is already maximal and stays maximal. */
  if (view()) {
    bool newInputViewHeightIsMaximal = static_cast<ContentView *>(view())->expressionField()->heightIsMaximal();
    if (!m_inputViewHeightIsMaximal || !newInputViewHeightIsMaximal) {
      m_inputViewHeightIsMaximal = newInputViewHeightIsMaximal;
      reloadView();
    }
  }
}

TextFieldDelegateApp * EditExpressionController::textFieldDelegateApp() {
  return (App *)app();
}

ExpressionFieldDelegateApp * EditExpressionController::expressionFieldDelegateApp() {
  return (App *)app();
}

View * EditExpressionController::loadView() {
  return new ContentView(this, (TableView *)m_historyController->view(), this, this);
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
  /* The input text store in m_cacheBuffer might have beed correct the first
   * time but then be too long when replacing ans in another context */
  if (!calculationApp->textInputIsCorrect(m_cacheBuffer)) {
    return true;
  }
  m_calculationStore->push(m_cacheBuffer, calculationApp->localContext());
  m_historyController->reload();
  ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  return true;
}


bool EditExpressionController::inputViewDidFinishEditing(const char * text, ExpressionLayout * layout) {
  App * calculationApp = (App *)app();
  if (layout == nullptr) {
    assert(text);
    strlcpy(m_cacheBuffer, text, Calculation::k_printedExpressionSize);
  } else {
    assert(layout);
    layout->writeTextInBuffer(m_cacheBuffer, Calculation::k_printedExpressionSize);
  }
  m_calculationStore->push(m_cacheBuffer, calculationApp->localContext());
  m_historyController->reload();
  ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  ((ContentView *)view())->expressionField()->setEditing(true);
  ((ContentView *)view())->expressionField()->setText("");
  return true;
}

bool EditExpressionController::inputViewDidAbortEditing(const char * text) {
  if (text != nullptr) {
    ((ContentView *)view())->expressionField()->setEditing(true, true);
    ((ContentView *)view())->expressionField()->setText(text);
  }
  return false;
}

void EditExpressionController::viewDidDisappear() {
  DynamicViewController::viewDidDisappear();
  m_historyController->viewDidDisappear();
}

}
