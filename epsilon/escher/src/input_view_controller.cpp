#include <assert.h>
#include <escher/container.h>
#include <escher/input_view_controller.h>
#include <escher/palette.h>

namespace Escher {

InputViewController::ExpressionInputBarController::ExpressionInputBarController(
    InputViewController* inputViewController)
    : ViewController(inputViewController),
      m_expressionInputBar(this, inputViewController) {}

void InputViewController::ExpressionInputBarController::
    handleResponderChainEvent(Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    App::app()->setFirstResponder(m_expressionInputBar.layoutField());
  } else {
    ViewController::handleResponderChainEvent(event);
  }
}

InputViewController::InputViewController(
    Responder* parentResponder, ViewController* child,
    LayoutFieldDelegate* layoutFieldDelegate)
    : ModalViewController(parentResponder, child),
      m_expressionInputBarController(this),
      m_successAction(Invocation(nullptr, nullptr)),
      m_failureAction(Invocation(nullptr, nullptr)),
      m_layoutFieldDelegate(layoutFieldDelegate) {}

void InputViewController::edit(Ion::Events::Event event, void* context,
                               Invocation::Action successAction,
                               Invocation::Action failureAction) {
  m_successAction = Invocation(successAction, context);
  m_failureAction = Invocation(failureAction, context);
  displayModalViewController(&m_expressionInputBarController, 1.0f, 1.0f);
  m_expressionInputBarController.layoutField()->handleEvent(event);
}

bool InputViewController::isEditing() {
  return m_expressionInputBarController.layoutField()->isEditing();
}

void InputViewController::abortEditionAndDismiss() {
  m_expressionInputBarController.layoutField()->clearAndSetEditing(false);
  dismissModal();
}

bool InputViewController::layoutFieldDidReceiveEvent(LayoutField* layoutField,
                                                     Ion::Events::Event event) {
  return m_layoutFieldDelegate->layoutFieldDidReceiveEvent(layoutField, event);
}

bool InputViewController::layoutFieldDidFinishEditing(
    LayoutField* layoutField, Ion::Events::Event event) {
  assert(!layoutField->isEditing());
  if (m_successAction.perform(this)) {
    dismissModal();
    m_layoutFieldDelegate->layoutFieldDidFinishEditing(layoutField, event);
    layoutField->clearLayout();
    return true;
  }
  return false;
}

void InputViewController::layoutFieldDidAbortEditing(LayoutField* layoutField) {
  m_failureAction.perform(this);
  dismissModal();
  m_layoutFieldDelegate->layoutFieldDidAbortEditing(layoutField);
}

void InputViewController::layoutFieldDidChangeSize(LayoutField* layoutField) {
  if (m_expressionInputBarController.layoutField()
          ->inputViewHeightDidChange()) {
    /* Reload the whole view only if the LayoutField's height did actually
     * change. */
    reloadModal();
  } else {
    /* The input view is already at maximal size so we do not need to relayout
     * the view underneath, but the view inside the input view might still need
     * to be relayouted.
     * We force the relayout because the frame stays the same but we need to
     * propagate a relayout to the content of the field scroll view. */
    m_expressionInputBarController.layoutField()->layoutSubviews(true);
  }
}

void InputViewController::updateRepetitionIndexes(LayoutField* layoutField,
                                                  Ion::Events::Event event) {
  return m_layoutFieldDelegate->updateRepetitionIndexes(layoutField, event);
}

}  // namespace Escher
