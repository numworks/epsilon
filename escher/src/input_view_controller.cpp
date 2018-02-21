#include <escher/input_view_controller.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <assert.h>

InputViewController::TextFieldController::ContentView::ContentView(Responder * parentResponder, TextFieldDelegate * textFieldDelegate) :
  Responder(parentResponder),
  View(),
  m_textField(this, m_textBody, m_textBody, TextField::maxBufferSize(), textFieldDelegate, false)
{
  m_textBody[0] = 0;
}

void InputViewController::TextFieldController::ContentView::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

TextField * InputViewController::TextFieldController::ContentView::textField() {
  return &m_textField;
}

void  InputViewController::TextFieldController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GreyMiddle);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_textMargin, bounds().height()-k_separatorThickness), m_textField.backgroundColor());
}

KDSize InputViewController::TextFieldController::ContentView::minimalSizeForOptimalDisplay() const {
  return KDSize(0, k_inputHeight);
}

int  InputViewController::TextFieldController::ContentView::numberOfSubviews() const {
  return 1;
}

View * InputViewController::TextFieldController::ContentView::subviewAtIndex(int index) {
  return &m_textField;
}

void  InputViewController::TextFieldController::ContentView::layoutSubviews() {
  m_textField.setFrame(KDRect(k_textMargin, k_separatorThickness, bounds().width()-k_textMargin, bounds().height()));
}

InputViewController::TextFieldController::TextFieldController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate) :
  ViewController(parentResponder),
  m_view(this, textFieldDelegate)
{
}

View * InputViewController::TextFieldController::view() {
  return &m_view;
}

void InputViewController::TextFieldController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_view);
}

TextField * InputViewController::TextFieldController::textField() {
  return m_view.textField();
}

InputViewController::InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate) :
  ModalViewController(parentResponder, child),
  m_textFieldController(this, this),
  m_successAction(Invocation(nullptr, nullptr)),
  m_failureAction(Invocation(nullptr, nullptr)),
  m_textFieldDelegate(textFieldDelegate)
{
}

const char * InputViewController::textBody() {
  return m_textFieldController.textField()->text();
}

void InputViewController::edit(Responder * caller, Ion::Events::Event event, void * context, const char * initialText, Invocation::Action successAction, Invocation::Action failureAction) {
  m_successAction = Invocation(successAction, context);
  m_failureAction = Invocation(failureAction, context);
  displayModalViewController(&m_textFieldController, 1.0f, 1.0f);
  m_textFieldController.textField()->handleEvent(event);
  if (initialText != nullptr) {
    m_textFieldController.textField()->handleEventWithText(initialText);
  }
}

void InputViewController::abortTextFieldEditionAndDismiss() {
  m_textFieldController.textField()->setEditing(false);
  dismissModalViewController();
}

bool InputViewController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool InputViewController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  m_successAction.perform(this);
  dismissModalViewController();
  return true;
}

bool InputViewController::textFieldDidAbortEditing(TextField * textField, const char * text) {
  m_failureAction.perform(this);
  dismissModalViewController();
  return true;
}

bool InputViewController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  return m_textFieldDelegate->textFieldDidReceiveEvent(textField, event);
}

Toolbox * InputViewController::toolboxForTextInput(TextInput * input) {
  return m_textFieldDelegate->toolboxForTextInput(input);
}
