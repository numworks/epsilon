#include "editable_value_cell.h"
#include "../../apps_container.h"
#include <assert.h>
#include "../app.h"

namespace Graph {

EditableValueCell::EditableValueCell(char * draftTextBuffer) :
  EvenOddCell(),
  Responder(nullptr),
  m_textField(TextField(this, m_textBody, draftTextBuffer, 255, nullptr, 1.0f, 0.5f, KDColorBlack, KDColorWhite))
{
}

void EditableValueCell::setDelegate(TextFieldDelegate * delegate) {
  m_textField.setTextFieldDelegate(delegate);
}

void EditableValueCell::reloadCell() {
  EvenOddCell::reloadCell();
  m_textField.setBackgroundColor(backgroundColor());
}

const char * EditableValueCell::text() const {
  return m_textField.text();
}

void EditableValueCell::setText(const char * textContent) {
  m_textField.setText(textContent);
}

int EditableValueCell::numberOfSubviews() const {
  return 1;
}

View * EditableValueCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textField;
}

void EditableValueCell::layoutSubviews() {
  m_textField.setFrame(bounds());
}

void EditableValueCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

void EditableValueCell::setEditing(bool isEditing) {
  m_textField.setEditing(isEditing);
}

}
