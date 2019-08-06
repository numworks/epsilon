#include <escher/input_view_controller.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <assert.h>

InputViewController::ExpressionFieldController::ExpressionFieldController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  ViewController(parentResponder),
  m_expressionField(this, m_textBuffer, k_bufferLength, inputEventHandlerDelegate, textFieldDelegate, layoutFieldDelegate)
{
  m_textBuffer[0] = 0;
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
  m_layoutFieldDelegate(layoutFieldDelegate),
  m_inputViewHeightIsMaximal(false)
{
}

const char * InputViewController::textBody() {
  return m_expressionFieldController.expressionField()->text();
}

void InputViewController::edit(Responder * caller, Ion::Events::Event event, void * context, const char * initialText, Invocation::Action successAction, Invocation::Action failureAction) {
  m_successAction = Invocation(successAction, context);
  m_failureAction = Invocation(failureAction, context);
  displayModalViewController(&m_expressionFieldController, 1.0f, 1.0f);
  if (initialText != nullptr) {
    m_expressionFieldController.expressionField()->setText(initialText);
  }
  m_expressionFieldController.expressionField()->handleEvent(event);
}

void InputViewController::abortEditionAndDismiss() {
  m_expressionFieldController.expressionField()->setEditing(false);
  dismissModalViewController();
}

bool InputViewController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool InputViewController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (inputViewDidFinishEditing()) {
    m_textFieldDelegate->textFieldDidFinishEditing(textField, text, event);
    return true;
  }
  return false;
}

bool InputViewController::textFieldDidAbortEditing(TextField * textField) {
  inputViewDidAbortEditing();
  m_textFieldDelegate->textFieldDidAbortEditing(textField);
  return true;
}

bool InputViewController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
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
  /* Reload the view only if the ExpressionField height actually changes, i.e.
   * not if the height is already maximal and stays maximal. */
  bool newInputViewHeightIsMaximal = m_expressionFieldController.expressionField()->heightIsMaximal();
  if (!m_inputViewHeightIsMaximal || !newInputViewHeightIsMaximal) {
    m_inputViewHeightIsMaximal = newInputViewHeightIsMaximal;
    reloadModalViewController();
  }
}

Toolbox * InputViewController::toolboxForInputEventHandler(InputEventHandler * handler) {
  return m_inputEventHandlerDelegate->toolboxForInputEventHandler(handler);
}

NestedMenuController * InputViewController::variableBoxForInputEventHandler(InputEventHandler * handler) {
  return m_inputEventHandlerDelegate->variableBoxForInputEventHandler(handler);
}

bool InputViewController::inputViewDidFinishEditing() {
  if (m_successAction.perform(this)) {
    dismissModalViewController();
    return true;
  }
  return false;
}

void InputViewController::inputViewDidAbortEditing() {
  m_failureAction.perform(this);
  dismissModalViewController();
}
