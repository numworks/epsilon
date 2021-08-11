#include "layout_cell_with_editable_text_with_message.h"

#include <escher/container.h>

namespace Probability {

LayoutCellWithEditableTextWithMessage::LayoutCellWithEditableTextWithMessage(
    Escher::Responder * parent,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate) :
    Responder(parent),
    ChainedTextFieldDelegate(textFieldDelegate),
    m_textField(this,
                m_textBody,
                sizeof(m_textBody),
                Escher::TextField::maxBufferSize(),
                inputEventHandlerDelegate,
                this,
                KDFont::LargeFont,
                1.) {
  m_textBody[0] = '\0';
}

void LayoutCellWithEditableTextWithMessage::didBecomeFirstResponder() {
  Escher::Container::activeApp()->setFirstResponder(&m_textField);
}

void LayoutCellWithEditableTextWithMessage::setAccessoryText(const char * text) {
  m_textField.setText(text);
  layoutSubviews();
}

void LayoutCellWithEditableTextWithMessage::setHighlighted(bool highlight) {
  LayoutCellWithSubMessage::setHighlighted(highlight);
  KDColor color = highlight ? Escher::Palette::Select : backgroundColor();
  m_textField.setBackgroundColor(color);
}

void LayoutCellWithEditableTextWithMessage::textFieldDidStartEditing(
    Escher::TextField * textField) {
  // Relayout to hide sublabel
  layoutSubviews();
  ChainedTextFieldDelegate::textFieldDidStartEditing(textField);
}

bool LayoutCellWithEditableTextWithMessage::textFieldDidFinishEditing(Escher::TextField * textField,
                                                                      const char * text,
                                                                      Ion::Events::Event event) {
  // Relayout to show sublabel
  layoutSubviews();
  return ChainedTextFieldDelegate::textFieldDidFinishEditing(textField, text, event);
}

bool LayoutCellWithEditableTextWithMessage::textFieldDidAbortEditing(
    Escher::TextField * textField) {
  // Relayout to show sublabel
  layoutSubviews();
  return ChainedTextFieldDelegate::textFieldDidAbortEditing(textField);
}

void LayoutCellWithEditableTextWithMessage::setDelegates(
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate) {
  m_textField.setInputEventHandlerDelegate(inputEventHandlerDelegate);
  ChainedTextFieldDelegate::setTextFieldDelegate(textFieldDelegate);
}

}  // namespace Probability
