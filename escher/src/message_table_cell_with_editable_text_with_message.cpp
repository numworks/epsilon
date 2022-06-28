#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithEditableTextWithMessage::MessageTableCellWithEditableTextWithMessage(
    Responder * parentResponder,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    TextFieldDelegate * textFieldDelegate,
    I18n::Message message) :
      MessageTableCellWithEditableText(parentResponder, inputEventHandlerDelegate, this, message),
      ChainedTextFieldDelegate(textFieldDelegate),
      m_subLabelView(KDFont::Size::Small,
                     (I18n::Message)0,
                     KDContext::k_alignLeft,
                     KDContext::k_alignCenter,
                     Palette::GrayDark) {
}

void MessageTableCellWithEditableTextWithMessage::setSubLabelMessage(I18n::Message textBody) {
  m_subLabelView.setMessage(textBody);
  reloadCell();
}

void MessageTableCellWithEditableTextWithMessage::setHighlighted(bool highlight) {
  MessageTableCellWithEditableText::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithEditableTextWithMessage::textFieldDidStartEditing(TextField * textField) {
  // Relayout to hide sublabel
  layoutSubviews();
  ChainedTextFieldDelegate::textFieldDidStartEditing(textField);
}

bool MessageTableCellWithEditableTextWithMessage::textFieldDidFinishEditing(
    TextField * textField,
    const char * text,
    Ion::Events::Event event) {
  bool success = ChainedTextFieldDelegate::textFieldDidFinishEditing(textField, text, event);
  if (success) {
    // Relayout to show sublabel
    layoutSubviews();
  }
  return success;
}

bool MessageTableCellWithEditableTextWithMessage::textFieldDidAbortEditing(TextField * textField) {
  // Relayout to show sublabel
  layoutSubviews();
  return ChainedTextFieldDelegate::textFieldDidAbortEditing(textField);
}

void MessageTableCellWithEditableTextWithMessage::setDelegates(
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    TextFieldDelegate * textFieldDelegate) {
  textField()->setInputEventHandlerDelegate(inputEventHandlerDelegate);
  ChainedTextFieldDelegate::setTextFieldDelegate(textFieldDelegate);
}

}  // namespace Escher
