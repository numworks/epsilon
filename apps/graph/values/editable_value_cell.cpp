#include "editable_value_cell.h"
#include <assert.h>
#include "../app.h"

namespace Graph {

EditableValueCell::EditableValueCell() :
  EvenOddCell(),
  Responder(nullptr),
  m_bufferTextView(BufferTextView(1.0f, 0.5f)),
  m_textField(TextField(this, m_textBody, 255, nullptr)),
  m_isEditing(false),
  m_successAction(Invocation(nullptr, nullptr))
{
}

void EditableValueCell::reloadCell() {
  EvenOddCell::reloadCell();
  m_bufferTextView.setBackgroundColor(backgroundColor());
}

void EditableValueCell::setText(const char * textContent) {
  m_bufferTextView.setText(textContent);
}

const char * EditableValueCell::editedText() const {
  return m_textField.text();
}

int EditableValueCell::numberOfSubviews() const {
  return 1;
}

View * EditableValueCell::subviewAtIndex(int index) {
  assert(index == 0);
  if (m_isEditing) {
    return &m_textField;
  }
  return &m_bufferTextView;
}

void EditableValueCell::layoutSubviews() {
  m_bufferTextView.setFrame(bounds());
  m_textField.setFrame(bounds());
}

bool EditableValueCell::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    m_isEditing = false;
    m_successAction.perform(this);
    app()->setFirstResponder(parentResponder());
    return true;
  }
  m_isEditing = false;
  app()->setFirstResponder(parentResponder());
  return false;
}


void EditableValueCell::editValue(const char * initialText, int cursorPosition, void * context, Invocation::Action successAction) {
  markRectAsDirty(bounds());
  App * myApp = (App *)app();
  m_textField.setTextFieldDelegate(myApp);
  app()->setFirstResponder(&m_textField);
  m_isEditing = true;
  m_textField.setText(initialText);
  m_textField.setCursorLocation(cursorPosition);
  m_successAction = Invocation(successAction, context);
}

}
