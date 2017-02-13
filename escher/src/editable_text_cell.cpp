#include <escher/editable_text_cell.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <assert.h>

EditableTextCell::EditableTextCell(Responder * parentResponder, TextFieldDelegate * delegate, char * draftTextBuffer,
   KDText::FontSize size, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  TableViewCell(),
  Responder(parentResponder),
  m_textField(TextField(this, m_textBody, draftTextBuffer, 255, delegate, size, horizontalAlignment, verticalAlignment, textColor, backgroundColor))
{
}

TextField * EditableTextCell::textField() {
  return &m_textField;
}

void EditableTextCell::setHighlighted(bool highlight) {
  TableViewCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_textField.setBackgroundColor(backgroundColor);
}

const char * EditableTextCell::text() const {
  return m_textField.text();
}

void EditableTextCell::setText(const char * text) {
  m_textField.setText(text);
}

int EditableTextCell::numberOfSubviews() const {
  return 1;
}

View * EditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textField;
}

void EditableTextCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_textField.setFrame(KDRect(k_separatorThickness, (height - k_textHeight)/2, width - k_separatorThickness, k_textHeight));
}

void EditableTextCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

bool EditableTextCell::isEditing() {
  return m_textField.isEditing();
}

void EditableTextCell::setEditing(bool isEditing) {
  m_textField.setEditing(isEditing);
}

