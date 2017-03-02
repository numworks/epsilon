#include <escher/pointer_table_cell_with_editable_text.h>
#include <escher/palette.h>
#include <escher/app.h>

PointerTableCellWithEditableText::PointerTableCellWithEditableText(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, char * draftTextBuffer, char * label) :
  Responder(parentResponder),
  PointerTableCell(label),
  m_textField(TextField(this, m_textBody, draftTextBuffer, 255, textFieldDelegate, KDText::FontSize::Large, 1.0f, 0.5f))
{
}

View * PointerTableCellWithEditableText::accessoryView() const {
  return (View *)&m_textField;
}

const char * PointerTableCellWithEditableText::editedText() const {
  return m_textField.text();
}

void PointerTableCellWithEditableText::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

void PointerTableCellWithEditableText::setEditing(bool isEditing) {
  m_textField.setEditing(isEditing);
}

void PointerTableCellWithEditableText::setHighlighted(bool highlight) {
  PointerTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_textField.setBackgroundColor(backgroundColor);
}

void PointerTableCellWithEditableText::setAccessoryText(const char * text) {
  m_textField.setText(text);
  layoutSubviews();
}

void PointerTableCellWithEditableText::setTextColor(KDColor color) {
  m_textField.setTextColor(color);
  PointerTableCell::setTextColor(color);
}
