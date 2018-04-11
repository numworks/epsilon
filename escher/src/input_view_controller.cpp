#include <escher/input_view_controller.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <poincare/src/layout/horizontal_layout.h>
#include <assert.h>

InputViewController::EditableExpressionViewController::EditableExpressionViewController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, ScrollableExpressionViewWithCursorDelegate * scrollableExpressionViewWithCursorDelegate) :
  ViewController(parentResponder),
  m_editableExpressionView(this, textFieldDelegate, scrollableExpressionViewWithCursorDelegate)
{
}

void InputViewController::EditableExpressionViewController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_editableExpressionView);
}

InputViewController::InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate, ScrollableExpressionViewWithCursorDelegate * scrollableExpressionViewWithCursorDelegate) :
  ModalViewController(parentResponder, child),
  m_editableExpressionViewController(this, this, this),
  m_successAction(Invocation(nullptr, nullptr)),
  m_failureAction(Invocation(nullptr, nullptr)),
  m_textFieldDelegate(textFieldDelegate),
  m_scrollableExpressionViewWithCursorDelegate(scrollableExpressionViewWithCursorDelegate),
  m_inputViewHeightIsMaximal(false)
{
}

const char * InputViewController::textBody() {
  return m_editableExpressionViewController.editableExpressionView()->text();
}

void InputViewController::edit(Responder * caller, Ion::Events::Event event, void * context, const char * initialText, Invocation::Action successAction, Invocation::Action failureAction) {
  m_successAction = Invocation(successAction, context);
  m_failureAction = Invocation(failureAction, context);
  displayModalViewController(&m_editableExpressionViewController, 1.0f, 1.0f);
  m_editableExpressionViewController.editableExpressionView()->handleEvent(event);
  if (initialText != nullptr) {
    m_editableExpressionViewController.editableExpressionView()->setText(initialText);
  }
}

void InputViewController::abortEditionAndDismiss() {
  m_editableExpressionViewController.editableExpressionView()->setEditing(false);
  dismissModalViewController();
}

bool InputViewController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool InputViewController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  return inputViewDidFinishEditing();
}

bool InputViewController::textFieldDidAbortEditing(TextField * textField) {
  return inputViewDidAbortEditing();
}

bool InputViewController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  return m_textFieldDelegate->textFieldDidReceiveEvent(textField, event);
}

Toolbox * InputViewController::toolboxForTextInput(TextInput * input) {
  return m_textFieldDelegate->toolboxForTextInput(input);
}

bool InputViewController::scrollableExpressionViewWithCursorShouldFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool InputViewController::scrollableExpressionViewWithCursorDidReceiveEvent(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) {
  return m_scrollableExpressionViewWithCursorDelegate->scrollableExpressionViewWithCursorDidReceiveEvent(scrollableExpressionViewWithCursor, event);
}

bool InputViewController::scrollableExpressionViewWithCursorDidFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, const char * text, Ion::Events::Event event) {
  return inputViewDidFinishEditing();
}

bool InputViewController::scrollableExpressionViewWithCursorDidAbortEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) {
  return inputViewDidAbortEditing();
}

void InputViewController::scrollableExpressionViewWithCursorDidChangeSize(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) {
  // Reload the view only if the EditableExpressionView height actually changes,
  // i.e. not if the height is already maximal and stays maximal.
  bool newInputViewHeightIsMaximal = m_editableExpressionViewController.editableExpressionView()->heightIsMaximal();
  if (!m_inputViewHeightIsMaximal || !newInputViewHeightIsMaximal) {
    m_inputViewHeightIsMaximal = newInputViewHeightIsMaximal;
    reloadView();
  }
}

Toolbox * InputViewController::toolboxForScrollableExpressionViewWithCursor(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) {
  return m_scrollableExpressionViewWithCursorDelegate->toolboxForScrollableExpressionViewWithCursor(scrollableExpressionViewWithCursor);
}

bool InputViewController::inputViewDidFinishEditing() {
  m_successAction.perform(this);
  dismissModalViewController();
  return true;
}

bool InputViewController::inputViewDidAbortEditing() {
  m_failureAction.perform(this);
  dismissModalViewController();
  return true;
}
