#include "edit_expression_controller.h"

#include <assert.h>
#include <ion/display.h>
#include <poincare/preferences.h>

#include "app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

EditExpressionController::ContentView::ContentView(
    Responder *parentResponder, CalculationSelectableTableView *subview,
    InputEventHandlerDelegate *inputEventHandlerDelegate,
    LayoutFieldDelegate *layoutFieldDelegate)
    : View(),
      m_mainView(subview),
      m_expressionInputBar(parentResponder, inputEventHandlerDelegate,
                           layoutFieldDelegate) {}

View *EditExpressionController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  if (index == 0) {
    return m_mainView;
  }
  assert(index == 1);
  return &m_expressionInputBar;
}

void EditExpressionController::ContentView::layoutSubviews(bool force) {
  KDCoordinate inputViewFrameHeight =
      m_expressionInputBar.minimalSizeForOptimalDisplay().height();
  KDRect mainViewFrame(0, 0, bounds().width(),
                       bounds().height() - inputViewFrameHeight);
  setChildFrame(m_mainView, mainViewFrame, force);
  KDRect inputViewFrame(0, bounds().height() - inputViewFrameHeight,
                        bounds().width(), inputViewFrameHeight);
  setChildFrame(&m_expressionInputBar, inputViewFrame, force);
}

void EditExpressionController::ContentView::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

EditExpressionController::EditExpressionController(
    Responder *parentResponder,
    InputEventHandlerDelegate *inputEventHandlerDelegate,
    HistoryController *historyController, CalculationStore *calculationStore)
    : ViewController(parentResponder),
      m_historyController(historyController),
      m_calculationStore(calculationStore),
      m_contentView(this,
                    static_cast<CalculationSelectableTableView *>(
                        m_historyController->view()),
                    inputEventHandlerDelegate, this) {
  clearWorkingBuffer();
}

void EditExpressionController::insertTextBody(const char *text) {
  Container::activeApp()->setFirstResponder(this);
  m_contentView.expressionField()->handleEventWithText(text, false, true);
  memoizeInput();
}

void EditExpressionController::didBecomeFirstResponder() {
  m_contentView.mainView()->scrollToBottom();
  m_contentView.expressionField()->setEditing(true);
  Container::activeApp()->setFirstResponder(m_contentView.expressionField());
}

void EditExpressionController::restoreInput() {
  App::Snapshot *snap = App::app()->snapshot();
  m_contentView.expressionField()->restoreContent(
      snap->cacheBuffer(), *snap->cacheBufferInformationAddress(),
      snap->cacheCursorOffset(), snap->cacheCursorPosition());
}

void EditExpressionController::memoizeInput() {
  App::Snapshot *snap = App::app()->snapshot();
  *snap->cacheBufferInformationAddress() =
      m_contentView.expressionField()->dumpContent(
          snap->cacheBuffer(), k_cacheBufferSize, snap->cacheCursorOffset(),
          snap->cacheCursorPosition());
}

void EditExpressionController::viewWillAppear() {
  m_historyController->viewWillAppear();
}

bool EditExpressionController::textFieldDidReceiveEvent(
    AbstractTextField *textField, Ion::Events::Event event) {
  bool shouldDuplicateLastCalculation = textField->isEditing() &&
                                        textField->shouldFinishEditing(event) &&
                                        textField->draftTextLength() == 0;
  if (inputViewDidReceiveEvent(event, shouldDuplicateLastCalculation)) {
    return true;
  }
  return textFieldDelegateApp()->textFieldDidReceiveEvent(textField, event);
}

bool EditExpressionController::textFieldDidHandleEvent(
    Escher::AbstractTextField *textField, bool returnValue,
    bool textDidChange) {
  return inputViewDidHandleEvent(returnValue);
}

bool EditExpressionController::textFieldDidFinishEditing(
    AbstractTextField *textField, const char *text, Ion::Events::Event event) {
  return inputViewDidFinishEditing(text, nullptr);
}

bool EditExpressionController::textFieldDidAbortEditing(
    AbstractTextField *textField) {
  return inputViewDidAbortEditing(textField->text());
}

bool EditExpressionController::layoutFieldDidReceiveEvent(
    ::LayoutField *layoutField, Ion::Events::Event event) {
  bool shouldDuplicateLastCalculation =
      layoutField->isEditing() && layoutField->shouldFinishEditing(event) &&
      layoutField->isEmpty();
  if (inputViewDidReceiveEvent(event, shouldDuplicateLastCalculation)) {
    return true;
  }
  return expressionFieldDelegateApp()->layoutFieldDidReceiveEvent(layoutField,
                                                                  event);
}

bool EditExpressionController::layoutFieldDidHandleEvent(
    Escher::LayoutField *layoutField, bool returnValue, bool layoutDidChange) {
  return inputViewDidHandleEvent(returnValue);
}

bool EditExpressionController::layoutFieldDidFinishEditing(
    ::LayoutField *layoutField, Layout layoutR, Ion::Events::Event event) {
  return inputViewDidFinishEditing(nullptr, layoutR);
}

bool EditExpressionController::layoutFieldDidAbortEditing(
    ::LayoutField *layoutField) {
  return inputViewDidAbortEditing(nullptr);
}

void EditExpressionController::layoutFieldDidChangeSize(
    ::LayoutField *layoutField) {
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

bool EditExpressionController::inputViewDidReceiveEvent(
    Ion::Events::Event event, bool shouldDuplicateLastCalculation) {
  if (shouldDuplicateLastCalculation && m_workingBuffer[0] != 0) {
    /* The input text store in m_workingBuffer might have been correct the first
     * time but then be too long when replacing ans in another context */
    Shared::TextFieldDelegateApp *myApp = textFieldDelegateApp();
    if (!myApp->isAcceptableText(m_workingBuffer)) {
      return true;
    }
    if (m_calculationStore
            ->push(m_workingBuffer, myApp->localContext(),
                   HistoryViewCell::Height)
            .pointer()) {
      m_historyController->reload();
      return true;
    }
  }
  if (event == Ion::Events::Up) {
    if (m_calculationStore->numberOfCalculations() > 0) {
      clearWorkingBuffer();
      m_contentView.expressionField()->setEditing(false);
      Container::activeApp()->setFirstResponder(m_historyController);
    }
    return true;
  }
  if (event == Ion::Events::Clear &&
      m_contentView.expressionField()->isEmpty()) {
    m_calculationStore->deleteAll();
    m_historyController->reload();
    return true;
  }
  return false;
}

bool EditExpressionController::inputViewDidHandleEvent(bool returnValue) {
  /* Memoize on all handled event, even if the text did not change, to properly
   * update the cursor position. */
  if (returnValue) {
    memoizeInput();
  }
  return returnValue;
}

bool EditExpressionController::inputViewDidFinishEditing(const char *text,
                                                         Layout layoutR) {
  Context *context = textFieldDelegateApp()->localContext();
  if (layoutR.isUninitialized()) {
    assert(text);
    strlcpy(m_workingBuffer, text, k_cacheBufferSize);
  } else {
    layoutR.serializeParsedExpression(m_workingBuffer, k_cacheBufferSize,
                                      context);
  }
  if (m_calculationStore
          ->push(m_workingBuffer, context, HistoryViewCell::Height)
          .pointer()) {
    m_historyController->reload();
    m_contentView.expressionField()->clearAndSetEditing(true);
    telemetryReportEvent("Input", m_workingBuffer);
    return true;
  }
  return false;
}

bool EditExpressionController::inputViewDidAbortEditing(const char *text) {
  if (text != nullptr) {
    m_contentView.expressionField()->clearAndSetEditing(true);
    m_contentView.expressionField()->setText(text);
  }
  return false;
}

}  // namespace Calculation
