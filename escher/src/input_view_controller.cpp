#include <escher/input_view_controller.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

InputViewController::ExpressionFieldController::ExpressionFieldController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  ViewController(parentResponder),
  m_expressionField(this, inputEventHandlerDelegate, textFieldDelegate, layoutFieldDelegate)
{
}

void InputViewController::ExpressionFieldController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_expressionField);
}

InputViewController::InputViewController(Responder * parentResponder, ViewController * child, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  ModalViewController(parentResponder, child),
  m_expressionFieldController(this, this, this, this),
  m_successAction(Invocation(nullptr, nullptr)),
  m_failureAction(Invocation(nullptr, nullptr)),
  m_inputEventHandlerDelegate(inputEventHandlerDelegate),
  m_textFieldDelegate(textFieldDelegate),
  m_layoutFieldDelegate(layoutFieldDelegate)
{
}

void InputViewController::edit(Ion::Events::Event event, void * context, Invocation::Action successAction, Invocation::Action failureAction) {
  m_successAction = Invocation(successAction, context);
  m_failureAction = Invocation(failureAction, context);
  displayModalViewController(&m_expressionFieldController, 1.0f, 1.0f);
  m_expressionFieldController.expressionField()->handleEvent(event);
}

bool InputViewController::isEditing() {
  return m_expressionFieldController.expressionField()->isEditing();
}

void InputViewController::abortEditionAndDismiss() {
  m_expressionFieldController.expressionField()->setEditing(false);
  dismissModal();
}

bool InputViewController::textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool InputViewController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  if (inputViewDidFinishEditing()) {
    m_textFieldDelegate->textFieldDidFinishEditing(textField, text, event);
    return true;
  }
  return false;
}

bool InputViewController::textFieldDidAbortEditing(AbstractTextField * textField) {
  inputViewDidAbortEditing();
  m_textFieldDelegate->textFieldDidAbortEditing(textField);
  return true;
}

bool InputViewController::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  return m_textFieldDelegate->textFieldDidReceiveEvent(textField, event);
}

bool InputViewController::layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool InputViewController::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  return m_layoutFieldDelegate->layoutFieldDidReceiveEvent(layoutField, event);
}

bool InputViewController::layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::Layout layoutR, Ion::Events::Event event) {
  if (inputViewDidFinishEditing()) {
    m_layoutFieldDelegate->layoutFieldDidFinishEditing(layoutField, layoutR, event);
    return true;
  }
  return false;
}

bool InputViewController::layoutFieldDidAbortEditing(LayoutField * layoutField) {
  inputViewDidAbortEditing();
  m_layoutFieldDelegate->layoutFieldDidAbortEditing(layoutField);
  return true;
}

void InputViewController::layoutFieldDidChangeSize(LayoutField * layoutField) {
  if (m_expressionFieldController.expressionField()->inputViewHeightDidChange()) {
    /* Reload the whole view only if the ExpressionField's height did actually
     * change. */
    reloadModal();
  } else {
    /* The input view is already at maximal size so we do not need to relayout
     * the view underneath, but the view inside the input view might still need
     * to be relayouted.
     * We force the relayout because the frame stays the same but we need to
     * propagate a relayout to the content of the field scroll view. */
    m_expressionFieldController.expressionField()->layoutSubviews(true);
  }
}

PervasiveBox * InputViewController::toolbox() {
  return m_inputEventHandlerDelegate->toolbox();
}

PervasiveBox * InputViewController::variableBox() {
  return m_inputEventHandlerDelegate->variableBox();
}

bool InputViewController::inputViewDidFinishEditing() {
  if (m_successAction.perform(this)) {
    dismissModal();
    return true;
  }
  return false;
}

void InputViewController::inputViewDidAbortEditing() {
  m_failureAction.perform(this);
  dismissModal();
}

}
