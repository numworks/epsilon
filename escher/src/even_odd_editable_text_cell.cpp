#include <escher/even_odd_editable_text_cell.h>
#include <escher/app.h>
#include <assert.h>

EvenOddEditableTextCell::EvenOddEditableTextCell(Responder * parentResponder, TextFieldDelegate * delegate, char * draftTextBuffer, KDText::FontSize size) :
  EvenOddCell(),
  Responder(parentResponder),
  m_editableCell(this, delegate, draftTextBuffer, size, 1.0f, 0.5f, KDColorBlack, KDColorWhite)
{
}

EditableTextCell * EvenOddEditableTextCell::editableTextCell() {
  return &m_editableCell;
}

void EvenOddEditableTextCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_editableCell.textField()->setBackgroundColor(backgroundColor());
}

void EvenOddEditableTextCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_editableCell.textField()->setBackgroundColor(backgroundColor());
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

bool EvenOddEditableTextCell::isEditing() {
  return m_editableCell.isEditing();
}

void EvenOddEditableTextCell::setEditing(bool isEditing) {
  m_editableCell.setEditing(isEditing);
}
