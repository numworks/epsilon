#include <escher/even_odd_editable_text_cell.h>
#include <escher/app.h>
#include <assert.h>

EvenOddEditableTextCell::EvenOddEditableTextCell(char * draftTextBuffer) :
  EvenOddCell(),
  Responder(nullptr),
  m_editableCell(this, nullptr, draftTextBuffer, 1.0f, 0.5f, KDColorBlack, KDColorWhite)
{
}

void EvenOddEditableTextCell::setDelegate(TextFieldDelegate * delegate) {
  m_editableCell.textfield()->setTextFieldDelegate(delegate);
}

void EvenOddEditableTextCell::reloadCell() {
  EvenOddCell::reloadCell();
  m_editableCell.textfield()->setBackgroundColor(backgroundColor());
}

const char * EvenOddEditableTextCell::text() const {
  return m_editableCell.text();
}

void EvenOddEditableTextCell::setText(const char * textContent) {
  m_editableCell.setText(textContent);
}

int EvenOddEditableTextCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddEditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_editableCell;
}

void EvenOddEditableTextCell::layoutSubviews() {
  m_editableCell.setFrame(bounds());
}

void EvenOddEditableTextCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_editableCell);
}

void EvenOddEditableTextCell::setEditing(bool isEditing) {
  m_editableCell.setEditing(isEditing);
}
