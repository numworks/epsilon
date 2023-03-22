#include <escher/message_table_cell_with_editable_text.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithEditableText::MessageTableCellWithEditableText(
    Responder* parentResponder,
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* textFieldDelegate, I18n::Message message)
    : Responder(parentResponder),
      MessageTableCell(message),
      m_textField(this, m_textBody, Poincare::PrintFloat::k_maxFloatCharSize,
                  inputEventHandlerDelegate, textFieldDelegate,
                  KDFont::Size::Large, KDContext::k_alignRight,
                  KDContext::k_alignCenter, KDColorBlack) {
  m_textBody[0] = '\0';
}

void MessageTableCellWithEditableText::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  m_textField.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithEditableText::setAccessoryText(const char* text) {
  m_textField.setText(text);
  layoutSubviews();
}

void MessageTableCellWithEditableText::setDelegates(
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* textFieldDelegate) {
  m_textField.setDelegates(inputEventHandlerDelegate, textFieldDelegate);
}

}  // namespace Escher
