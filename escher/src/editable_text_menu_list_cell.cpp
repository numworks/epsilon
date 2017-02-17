#include <escher/editable_text_menu_list_cell.h>
#include <escher/app.h>

EditableTextMenuListCell::EditableTextMenuListCell(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, char * draftTextBuffer, char * label) :
  Responder(parentResponder),
  MenuListCell(label),
  m_textField(TextField(this, m_textBody, draftTextBuffer, 255, textFieldDelegate, KDText::FontSize::Large, 1.0f, 0.5f))
{
}

View * EditableTextMenuListCell::accessoryView() const {
  return (View *)&m_textField;
}

const char * EditableTextMenuListCell::editedText() const {
  return m_textField.text();
}

void EditableTextMenuListCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

void EditableTextMenuListCell::setEditing(bool isEditing) {
  m_textField.setEditing(isEditing);
}

void EditableTextMenuListCell::setHighlighted(bool highlight) {
  MenuListCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_textField.setBackgroundColor(backgroundColor);
}

void EditableTextMenuListCell::setAccessoryText(const char * text) {
  m_textField.setText(text);
}

void EditableTextMenuListCell::setTextColor(KDColor color) {
  m_textField.setTextColor(color);
  MenuListCell::setTextColor(color);
}
