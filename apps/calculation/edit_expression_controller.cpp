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
    Responder *parentResponder, CalculationSelectableListView *subview,
    LayoutFieldDelegate *layoutFieldDelegate)
    : View(),
      m_mainView(subview),
      m_expressionInputBar(parentResponder, layoutFieldDelegate) {}

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
  markWholeFrameAsDirty();
}

EditExpressionController::EditExpressionController(
    Responder *parentResponder, HistoryController *historyController,
    CalculationStore *calculationStore)
    : ViewController(parentResponder),
      m_historyController(historyController),
      m_calculationStore(calculationStore),
      m_contentView(this,
                    static_cast<CalculationSelectableListView *>(
                        m_historyController->view()),
                    this) {
  clearWorkingBuffer();
}

void EditExpressionController::insertTextBody(const char *text) {
  App::app()->setFirstResponder(this);
  m_contentView.layoutField()->updateCursorBeforeInsertion();
  m_contentView.layoutField()->handleEventWithText(text, false, true);
}

void EditExpressionController::didBecomeFirstResponder() {
  m_contentView.mainView()->scrollToBottom();
  m_contentView.layoutField()->setEditing(true);
  App::app()->setFirstResponder(m_contentView.layoutField());
}

void EditExpressionController::restoreInput() {
  App::Snapshot *snap = App::app()->snapshot();
  m_contentView.layoutField()->restoreContent(
      snap->cacheBuffer(), *snap->cacheBufferInformationAddress(),
      snap->cacheCursorOffset(), snap->cacheCursorPosition());
  if (Poincare::Preferences::sharedPreferences->editionMode() ==
          Poincare::Preferences::EditionMode::Edition1D &&
      !m_contentView.layoutField()->layout().isCodePointsString()) {
    // Restored input in incompatible with edition mode.
    m_contentView.layoutField()->clearLayout();
    memoizeInput();
  }
}

void EditExpressionController::memoizeInput() {
  App::Snapshot *snap = App::app()->snapshot();
  *snap->cacheBufferInformationAddress() =
      m_contentView.layoutField()->dumpContent(
          snap->cacheBuffer(), k_cacheBufferSize, snap->cacheCursorOffset(),
          snap->cacheCursorPosition());
}

void EditExpressionController::viewWillAppear() {
  m_historyController->viewWillAppear();
}

bool EditExpressionController::layoutFieldDidReceiveEvent(
    ::LayoutField *layoutField, Ion::Events::Event event) {
  assert(m_contentView.layoutField() == layoutField);
  if (event == Ion::Events::Up) {
    if (m_calculationStore->numberOfCalculations() > 0) {
      clearWorkingBuffer();
      layoutField->setEditing(false);
      App::app()->setFirstResponder(m_historyController);
    }
    return true;
  }
  if (event == Ion::Events::Clear && layoutField->isEmpty()) {
    m_calculationStore->deleteAll();
    m_historyController->reload();
    return true;
  }
  return MathLayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField,
                                                             event);
}

void EditExpressionController::layoutFieldDidHandleEvent(
    Escher::LayoutField *layoutField) {
  assert(m_contentView.layoutField() == layoutField);
  /* Memoize on all handled event, even if the text did not change, to properly
   * update the cursor position. */
  memoizeInput();
}

bool EditExpressionController::layoutFieldDidFinishEditing(
    ::LayoutField *layoutField, Ion::Events::Event event) {
  assert(!layoutField->isEditing());
  assert(m_contentView.layoutField() == layoutField);
  Context *context = App::app()->localContext();
  if (layoutField->isEmpty()) {
    if (m_workingBuffer[0] != 0) {
      /* The input text store in m_workingBuffer might have been correct the
       * first time but then be too long when replacing ans in another context
       */
      if (!isAcceptableText(m_workingBuffer)) {
        App::app()->displayWarning(I18n::Message::SyntaxError);
      } else {
        pushCalculation(m_workingBuffer, context);
      }
    }
    return false;
  }
  if (!MathLayoutFieldDelegate::layoutFieldDidFinishEditing(layoutField,
                                                            event)) {
    return false;
  }
  Layout layout = layoutField->layout();
  assert(!layout.isUninitialized());
  layout.serializeParsedExpression(m_workingBuffer, k_cacheBufferSize, context);
  if (pushCalculation(m_workingBuffer, context)) {
    layoutField->clearAndSetEditing(true);
    telemetryReportEvent("Input", m_workingBuffer);
    return true;
  }
  return false;
}

void EditExpressionController::layoutFieldDidChangeSize(
    ::LayoutField *layoutField) {
  assert(m_contentView.layoutField() == layoutField);
  if (layoutField->inputViewHeightDidChange()) {
    /* Reload the whole view only if the LayoutField's height did actually
     * change. */
    reloadView();
  } else {
    /* The input view is already at maximal size so we do not need to relayout
     * the view underneath, but the view inside the input view might still need
     * to be relayouted.
     * We force the relayout because the frame stays the same but we need to
     * propagate a relayout to the content of the field scroll view. */
    layoutField->layoutSubviews(true);
  }
}

bool EditExpressionController::isAcceptableExpression(
    const Poincare::Expression expression) {
  /* Override MathLayoutFieldDelegate because Store is acceptable, and
   * ans has an expression. */
  {
    Expression ansExpression =
        App::app()->snapshot()->calculationStore()->ansExpression(
            App::app()->localContext());
    if (!ExpressionCanBeSerialized(expression, true, ansExpression,
                                   App::app()->localContext())) {
      return false;
    }
  }
  return !expression.isUninitialized();
}

void EditExpressionController::reloadView() {
  m_contentView.reload();
  m_historyController->reload();
}

bool EditExpressionController::pushCalculation(const char *text,
                                               Poincare::Context *context) {
  Calculation *calculation = m_calculationStore->push(text, context).pointer();
  if (calculation) {
    HistoryViewCell::ComputeCalculationHeights(calculation, context);
    m_historyController->reload();
    return true;
  }
  return false;
}

}  // namespace Calculation
