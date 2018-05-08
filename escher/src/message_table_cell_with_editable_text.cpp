#include <escher/message_table_cell_with_editable_text.h>
#include <escher/palette.h>
#include <escher/app.h>

MessageTableCellWithEditableText::MessageTableCellWithEditableText(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, char * draftTextBuffer, I18n::Message message) :
  Responder(parentResponder),
  MessageTableCell(message),
  m_textField(this, m_textBody, draftTextBuffer, TextField::maxBufferSize(), textFieldDelegate, true, KDText::FontSize::Large, 1.0f, 0.5f)
{
  m_textBody[0] = '\0';
}

View * MessageTableCellWithEditableText::accessoryView() const {
  return (View *)&m_textField;
}

const char * MessageTableCellWithEditableText::editedText() const {
  return m_textField.text();
}

void MessageTableCellWithEditableText::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

bool MessageTableCellWithEditableText::isEditing() {
  return m_textField.isEditing();
}

void MessageTableCellWithEditableText::setEditing(bool isEditing) {
  m_textField.setEditing(isEditing);
}

void MessageTableCellWithEditableText::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_textField.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithEditableText::setAccessoryText(const char * text) {
  m_textField.setText(text);
  layoutSubviews();
}

void MessageTableCellWithEditableText::setTextColor(KDColor color) {
  m_textField.setTextColor(color);
  MessageTableCell::setTextColor(color);
}

void MessageTableCellWithEditableText::layoutSubviews() {
  TableCell::layoutSubviews();
  KDSize textFieldSize = m_textField.minimalSizeForOptimalDisplay();
  KDSize labelSize = labelView()->minimalSizeForOptimalDisplay();
  /* Handle textfield that has no defined width (as their width evolves with
   * the length of edited text */
  textFieldSize = KDSize(bounds().width() - 2*k_separatorThickness - labelSize.width()-2*k_labelMargin-k_accessoryMargin, textFieldSize.height());
  m_textField.setFrame(KDRect(bounds().width() - textFieldSize.width() - k_separatorThickness-k_accessoryMargin, (bounds().height()-textFieldSize.height()-k_accessoryMargin)/2, textFieldSize.width(), textFieldSize.height()+k_accessoryMargin));
}
