#include <escher/message_table_cell_with_editable_text.h>
#include <escher/palette.h>
#include <escher/container.h>

MessageTableCellWithEditableText::MessageTableCellWithEditableText(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, I18n::Message message) :
  Responder(parentResponder),
  MessageTableCell(message),
  m_textField(this, m_textBody, Poincare::PrintFloat::k_maxFloatCharSize, TextField::maxBufferSize(), inputEventHandlerDelegate, textFieldDelegate, KDFont::LargeFont, 1.0f, 0.5f)
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
  Container::activeApp()->setFirstResponder(&m_textField);
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

void MessageTableCellWithEditableText::layoutSubviews(bool force) {
  TableCell::layoutSubviews(force);
  KDSize textFieldSize = m_textField.minimalSizeForOptimalDisplay();
  KDSize labelSize = labelView()->minimalSizeForOptimalDisplay();
  /* Handle textfield that has no defined width (as their width evolves with
   * the length of edited text */
  textFieldSize = KDSize(bounds().width() - 2*k_separatorThickness - labelSize.width()-2*labelMargin()-k_horizontalMargin, textFieldSize.height());
  m_textField.setFrame(KDRect(
    bounds().width() - textFieldSize.width() - k_separatorThickness-k_horizontalMargin,
    (bounds().height()-textFieldSize.height()-k_horizontalMargin)/2,
    textFieldSize.width(),
    textFieldSize.height()+k_horizontalMargin),
  force);
}
