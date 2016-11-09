#include <escher/input_view_controller.h>
#include <escher/app.h>
#include <assert.h>

InputViewController::TextFieldController::TextFieldController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate) :
  ViewController(parentResponder),
  m_textField(parentResponder, m_textBody, 255, textFieldDelegate)
{
  m_textBody[0] = 0;
}

View * InputViewController::TextFieldController::view() {
  return &m_textField;
}

void InputViewController::TextFieldController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

TextField * InputViewController::TextFieldController::textField() {
  return &m_textField;
}

InputViewController::InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate) :
  ModalViewController(parentResponder, child),
  m_textFieldController(TextFieldController(this, textFieldDelegate)),
  m_successAction(Invocation(nullptr, nullptr)),
  m_failureAction(Invocation(nullptr, nullptr))
{
}

const char * InputViewController::title() const {
  return "InputViewController";
}

const char * InputViewController::textBody() {
  return m_textFieldController.textField()->text();
}

void InputViewController::showInput() {
  displayModalViewController(&m_textFieldController, 1.0f, 1.0f);
}

void InputViewController::setTextBody(const char * text) {
 m_textFieldController.textField()->setText(text);
}

bool InputViewController::handleEvent(Ion::Events::Event event) {
  if (!isDisplayingModal()) {
    return false;
  }
  if (event == Ion::Events::OK) {
    m_successAction.perform(this);
    dismissModalViewController();
    return true;
  }
  if (event == Ion::Events::Back) {
    m_failureAction.perform(this);
    dismissModalViewController();
    return true;
  }
  return false;
}

void InputViewController::edit(Responder * caller, const char * initialContent, void * context, Invocation::Action successAction, Invocation::Action failureAction) {
  m_successAction = Invocation(successAction, context);
  m_failureAction = Invocation(failureAction, context);
  setTextBody(initialContent);
  showInput();
}
