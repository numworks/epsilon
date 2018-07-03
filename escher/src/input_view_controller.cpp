#include <escher/input_view_controller.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <poincare/horizontal_layout_node.h>
#include <assert.h>

InputViewController::ExpressionFieldController::ExpressionFieldController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  ViewController(parentResponder),
  m_layout(Poincare::HorizontalLayoutRef()),
  m_expressionField(this, m_textBuffer, k_bufferLength, m_layout, textFieldDelegate, layoutFieldDelegate)
{
  m_textBuffer[0] = 0;
}

void InputViewController::ExpressionFieldController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_expressionField);
}

InputViewController::InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  ModalViewController(parentResponder, child),
  m_expressionFieldController(this, this, this),
  m_successAction(Invocation(nullptr, nullptr)),
  m_failureAction(Invocation(nullptr, nullptr)),
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
  inputViewDidFinishEditing();
  m_textFieldDelegate->textFieldDidFinishEditing(textField, text, event);
  return true;
}

bool InputViewController::textFieldDidAbortEditing(TextField * textField) {
  inputViewDidAbortEditing();
  m_textFieldDelegate->textFieldDidAbortEditing(textField);
  return true;
}

bool InputViewController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  return m_textFieldDelegate->textFieldDidReceiveEvent(textField, event);
}

Toolbox * InputViewController::toolboxForTextInput(TextInput * input) {
  return m_textFieldDelegate->toolboxForTextInput(input);
}

bool InputViewController::layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool InputViewController::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  return m_layoutFieldDelegate->layoutFieldDidReceiveEvent(layoutField, event);
}

bool InputViewController::layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::LayoutRef layoutR, Ion::Events::Event event) {
  inputViewDidFinishEditing();
  m_layoutFieldDelegate->layoutFieldDidFinishEditing(layoutField, layoutR, event);
  return true;
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

Toolbox * InputViewController::toolboxForLayoutField(LayoutField * layoutField) {
  return m_layoutFieldDelegate->toolboxForLayoutField(layoutField);
}

void InputViewController::inputViewDidFinishEditing() {
  m_successAction.perform(this);
  dismissModalViewController();
}

void InputViewController::inputViewDidAbortEditing() {
  m_failureAction.perform(this);
  dismissModalViewController();
}
