#include <escher/input_view_controller.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <poincare/src/layout/horizontal_layout.h>
#include <assert.h>

InputViewController::ExpressionFieldController::ExpressionFieldController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, ExpressionLayoutFieldDelegate * expressionLayoutFieldDelegate) :
  ViewController(parentResponder),
  m_layout(new Poincare::HorizontalLayout()),
  m_expressionField(this, m_textBuffer, k_bufferLength, m_layout, textFieldDelegate, expressionLayoutFieldDelegate)
{
  m_textBuffer[0] = 0;
}

InputViewController::ExpressionFieldController::~ExpressionFieldController() {
  delete m_layout;
}

void InputViewController::ExpressionFieldController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_expressionField);
}

InputViewController::InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate, ExpressionLayoutFieldDelegate * expressionLayoutFieldDelegate) :
  ModalViewController(parentResponder, child),
  m_expressionFieldController(this, this, this),
  m_successAction(Invocation(nullptr, nullptr)),
  m_failureAction(Invocation(nullptr, nullptr)),
  m_textFieldDelegate(textFieldDelegate),
  m_expressionLayoutFieldDelegate(expressionLayoutFieldDelegate),
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

bool InputViewController::expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool InputViewController::expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  return m_expressionLayoutFieldDelegate->expressionLayoutFieldDidReceiveEvent(expressionLayoutField, event);
}

bool InputViewController::expressionLayoutFieldDidFinishEditing(ExpressionLayoutField * expressionLayoutField, Poincare::ExpressionLayout * layout, Ion::Events::Event event) {
  inputViewDidFinishEditing();
  m_expressionLayoutFieldDelegate->expressionLayoutFieldDidFinishEditing(expressionLayoutField, layout, event);
  return true;
}

bool InputViewController::expressionLayoutFieldDidAbortEditing(ExpressionLayoutField * expressionLayoutField) {
  inputViewDidAbortEditing();
  m_expressionLayoutFieldDelegate->expressionLayoutFieldDidAbortEditing(expressionLayoutField);
  return true;
}

void InputViewController::expressionLayoutFieldDidChangeSize(ExpressionLayoutField * expressionLayoutField) {
  /* Reload the view only if the ExpressionField height actually changes, i.e.
   * not if the height is already maximal and stays maximal. */
  bool newInputViewHeightIsMaximal = m_expressionFieldController.expressionField()->heightIsMaximal();
  if (!m_inputViewHeightIsMaximal || !newInputViewHeightIsMaximal) {
    m_inputViewHeightIsMaximal = newInputViewHeightIsMaximal;
    reloadModalViewController();
  }
}

Toolbox * InputViewController::toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) {
  return m_expressionLayoutFieldDelegate->toolboxForExpressionLayoutField(expressionLayoutField);
}

void InputViewController::inputViewDidFinishEditing() {
  m_successAction.perform(this);
  dismissModalViewController();
}

void InputViewController::inputViewDidAbortEditing() {
  m_failureAction.perform(this);
  dismissModalViewController();
}
