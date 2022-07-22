#include "expression_cell_with_editable_text_with_message.h"

#include <escher/container.h>

namespace Inference {

ExpressionCellWithEditableTextWithMessage::ExpressionCellWithEditableTextWithMessage(
    Escher::Responder * parent,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate) :
      Escher::ExpressionTableCellWithMessage(parent),
      ChainedTextFieldDelegate(textFieldDelegate),
      m_textField(this,
                  m_textBody,
                  sizeof(m_textBody),
                  Escher::TextField::maxBufferSize(),
                  inputEventHandlerDelegate,
                  this,
                  KDFont::Size::Large,
                  1.) {
  m_textBody[0] = '\0';
}

void ExpressionCellWithEditableTextWithMessage::didBecomeFirstResponder() {
  Escher::Container::activeApp()->setFirstResponder(&m_textField);
}

void ExpressionCellWithEditableTextWithMessage::setAccessoryText(const char * text) {
  m_textField.setText(text);
  layoutSubviews();
}

void ExpressionCellWithEditableTextWithMessage::setHighlighted(bool highlight) {
  Escher::ExpressionTableCellWithMessage::setHighlighted(highlight);
  KDColor color = highlight ? Escher::Palette::Select : backgroundColor();
  m_textField.setBackgroundColor(color);
}

void ExpressionCellWithEditableTextWithMessage::textFieldDidStartEditing(
    Escher::AbstractTextField * textField) {
  // Relayout to hide sublabel
  layoutSubviews();
  ChainedTextFieldDelegate::textFieldDidStartEditing(textField);
}

bool ExpressionCellWithEditableTextWithMessage::textFieldDidFinishEditing(
    Escher::AbstractTextField * textField,
    const char * text,
    Ion::Events::Event event) {
  bool success = ChainedTextFieldDelegate::textFieldDidFinishEditing(textField, text, event);
  if (success) {
    // Relayout to show sublabel
    layoutSubviews();
  }
  return success;
}

bool ExpressionCellWithEditableTextWithMessage::textFieldDidAbortEditing(
    Escher::AbstractTextField * textField) {
  // Relayout to show sublabel
  layoutSubviews();
  return ChainedTextFieldDelegate::textFieldDidAbortEditing(textField);
}

void ExpressionCellWithEditableTextWithMessage::setDelegates(
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate) {
  m_textField.setInputEventHandlerDelegate(inputEventHandlerDelegate);
  ChainedTextFieldDelegate::setTextFieldDelegate(textFieldDelegate);
}

}  // namespace Inference
