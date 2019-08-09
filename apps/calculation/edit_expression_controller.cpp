#include "edit_expression_controller.h"
#include "app.h"
#include <ion/display.h>
#include <poincare/preferences.h>
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Calculation {

EditExpressionController::ContentView::ContentView(Responder * parentResponder, TableView * subview, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  View(),
  m_mainView(subview),
  m_expressionField(parentResponder, inputEventHandlerDelegate, textFieldDelegate, layoutFieldDelegate)
{
}

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

EditExpressionController::EditExpressionController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, HistoryController * historyController, CalculationStore * calculationStore) :
  ViewController(parentResponder),
  m_historyController(historyController),
  m_calculationStore(calculationStore),
  m_contentView(this, (TableView *)m_historyController->view(), inputEventHandlerDelegate, this, this),
  m_inputViewHeightIsMaximal(false)
{
  m_cacheBuffer[0] = 0;
}

View * EditExpressionController::view() {
  return &m_contentView;
}

void EditExpressionController::insertTextBody(const char * text) {
  ((ContentView *)view())->expressionField()->handleEventWithText(text, false, true);
}

void EditExpressionController::didBecomeFirstResponder() {
  int lastRow = m_calculationStore->numberOfCalculations() > 0 ? m_calculationStore->numberOfCalculations()-1 : 0;
  m_historyController->scrollToCell(0, lastRow);
  ((ContentView *)view())->expressionField()->setEditing(true, false);
  Container::activeApp()->setFirstResponder(((ContentView *)view())->expressionField());
}

bool EditExpressionController::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  bool shouldDuplicateLastCalculation = textField->isEditing() && textField->shouldFinishEditing(event) && textField->draftTextLength() == 0;
  if (inputViewDidReceiveEvent(event, shouldDuplicateLastCalculation)) {
    return true;
  }
  return textFieldDelegateApp()->textFieldDidReceiveEvent(textField, event);
}

bool EditExpressionController::textFieldDidFinishEditing(::TextField * textField, const char * text, Ion::Events::Event event) {
  return inputViewDidFinishEditing(text, nullptr);
}

bool EditExpressionController::textFieldDidAbortEditing(::TextField * textField) {
  return inputViewDidAbortEditing(textField->text());
}

bool EditExpressionController::layoutFieldDidReceiveEvent(::LayoutField * layoutField, Ion::Events::Event event) {
  bool shouldDuplicateLastCalculation = layoutField->isEditing() && layoutField->shouldFinishEditing(event) && !layoutField->hasText();
  if (inputViewDidReceiveEvent(event, shouldDuplicateLastCalculation)) {
    return true;
  }
  return expressionFieldDelegateApp()->layoutFieldDidReceiveEvent(layoutField, event);
}

bool EditExpressionController::layoutFieldDidFinishEditing(::LayoutField * layoutField, Layout layoutR, Ion::Events::Event event) {
  return inputViewDidFinishEditing(nullptr, layoutR);
}

bool EditExpressionController::layoutFieldDidAbortEditing(::LayoutField * layoutField) {
  return inputViewDidAbortEditing(nullptr);
}

void EditExpressionController::layoutFieldDidChangeSize(::LayoutField * layoutField) {
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

void EditExpressionController::reloadView() {
  ((ContentView *)view())->reload();
  m_historyController->reload();
  if (m_historyController->numberOfRows() > 0) {
    ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  }
}

bool EditExpressionController::inputViewDidReceiveEvent(Ion::Events::Event event, bool shouldDuplicateLastCalculation) {
  if (shouldDuplicateLastCalculation && m_cacheBuffer[0] != 0) {
    /* The input text store in m_cacheBuffer might have been correct the first
     * time but then be too long when replacing ans in another context */
    Shared::TextFieldDelegateApp * myApp = textFieldDelegateApp();
    if (!myApp->isAcceptableText(m_cacheBuffer)) {
      return true;
    }
    m_calculationStore->push(m_cacheBuffer, myApp->localContext());
    m_historyController->reload();
    ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
    return true;
  }
  if (event == Ion::Events::Up) {
    if (m_calculationStore->numberOfCalculations() > 0) {
      m_cacheBuffer[0] = 0;
      ((ContentView *)view())->expressionField()->setEditing(false, false);
      Container::activeApp()->setFirstResponder(m_historyController);
    }
    return true;
  }
  return false;
}


bool EditExpressionController::inputViewDidFinishEditing(const char * text, Layout layoutR) {
  if (layoutR.isUninitialized()) {
    assert(text);
    strlcpy(m_cacheBuffer, text, k_cacheBufferSize);
  } else {
    layoutR.serializeParsedExpression(m_cacheBuffer, k_cacheBufferSize);
  }
  m_calculationStore->push(m_cacheBuffer, textFieldDelegateApp()->localContext());
  m_historyController->reload();
  ((ContentView *)view())->mainView()->scrollToCell(0, m_historyController->numberOfRows()-1);
  ((ContentView *)view())->expressionField()->setEditing(true, true);
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
  m_historyController->viewDidDisappear();
}

}
