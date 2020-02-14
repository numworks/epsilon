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

void EditExpressionController::ContentView::layoutSubviews(bool force) {
  KDCoordinate inputViewFrameHeight = m_expressionField.minimalSizeForOptimalDisplay().height();
  KDRect mainViewFrame(0, 0, bounds().width(), bounds().height() - inputViewFrameHeight);
  m_mainView->setFrame(mainViewFrame, force);
  KDRect inputViewFrame(0, bounds().height() - inputViewFrameHeight, bounds().width(), inputViewFrameHeight);
  m_expressionField.setFrame(inputViewFrame, force);
}

void EditExpressionController::ContentView::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

EditExpressionController::EditExpressionController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, HistoryController * historyController, CalculationStore * calculationStore) :
  ViewController(parentResponder),
  m_historyController(historyController),
  m_calculationStore(calculationStore),
  m_contentView(this, (TableView *)m_historyController->view(), inputEventHandlerDelegate, this, this)
{
  m_cacheBuffer[0] = 0;
}

void EditExpressionController::insertTextBody(const char * text) {
  m_contentView.expressionField()->handleEventWithText(text, false, true);
}

void EditExpressionController::didBecomeFirstResponder() {
  int lastRow = m_calculationStore->numberOfCalculations() > 0 ? m_calculationStore->numberOfCalculations()-1 : 0;
  m_historyController->scrollToCell(0, lastRow);
  m_contentView.expressionField()->setEditing(true, false);
  Container::activeApp()->setFirstResponder(m_contentView.expressionField());
}

void EditExpressionController::viewWillAppear() {
  m_historyController->viewWillAppear();
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
  if (m_contentView.expressionField()->inputViewHeightDidChange()) {
    /* Reload the whole view only if the ExpressionField's height did actually
     * change. */
    reloadView();
  } else {
    /* The input view is already at maximal size so we do not need to relayout
     * the view underneath, but the view inside the input view might still need
     * to be relayouted.
     * We force the relayout because the frame stays the same but we need to
     * propagate a relayout to the content of the field scroll view. */
    m_contentView.expressionField()->layoutSubviews(true);
  }
}

void EditExpressionController::reloadView() {
  m_contentView.reload();
  m_historyController->reload();
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
    return true;
  }
  if (event == Ion::Events::Up) {
    if (m_calculationStore->numberOfCalculations() > 0) {
      m_cacheBuffer[0] = 0;
      m_contentView.expressionField()->setEditing(false, false);
      Container::activeApp()->setFirstResponder(m_historyController);
    }
    return true;
  }
  return false;
}

bool EditExpressionController::inputViewDidFinishEditing(const char * text, Layout layoutR) {
  Context * context = textFieldDelegateApp()->localContext();
  if (layoutR.isUninitialized()) {
    assert(text);
    strlcpy(m_cacheBuffer, text, k_cacheBufferSize);
  } else {
    layoutR.serializeParsedExpression(m_cacheBuffer, k_cacheBufferSize, context);
  }
  m_calculationStore->push(m_cacheBuffer, context);
  m_historyController->reload();
  m_contentView.expressionField()->setEditing(true, true);
  telemetryReportEvent("Input", m_cacheBuffer);
  return true;
}

bool EditExpressionController::inputViewDidAbortEditing(const char * text) {
  if (text != nullptr) {
    m_contentView.expressionField()->setEditing(true, true);
    m_contentView.expressionField()->setText(text);
  }
  return false;
}

}
