#include "layout_cell_with_editable_text_with_message.h"

#include <escher/container.h>

namespace Probability {

LayoutCellWithEditableTextWithMessage::LayoutCellWithEditableTextWithMessage(
    Escher::Responder * parent,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Escher::TextFieldDelegate * textFieldDelegate) :
    Responder(parent),
    m_textField(this,
                m_textBody,
                sizeof(m_textBody),
                Escher::TextField::maxBufferSize(),
                inputEventHandlerDelegate,
                textFieldDelegate,
                KDFont::LargeFont,
                1.),
    m_messageTextView(KDFont::SmallFont) {
  m_textBody[0] = '\0';
  m_messageTextView.setTextColor(Escher::Palette::GrayDark);
}

void LayoutCellWithEditableTextWithMessage::didBecomeFirstResponder() {
  Escher::Container::activeApp()->setFirstResponder(&m_textField);
}

void LayoutCellWithEditableTextWithMessage::setAccessoryText(const char * text) {
  m_textField.setText(text);
  layoutSubviews();
}

void LayoutCellWithEditableTextWithMessage::setLayout(Poincare::Layout layout) {
  m_expressionView.setLayout(layout);
}

void LayoutCellWithEditableTextWithMessage::setSubLabelMessage(I18n::Message message) {
  m_messageTextView.setMessage(message);
}

void LayoutCellWithEditableTextWithMessage::setHighlighted(bool highlight) {
  Escher::TableCell::setHighlighted(highlight);
  KDColor color = highlight ? Escher::Palette::Select : backgroundColor();
  m_expressionView.setBackgroundColor(color);
  m_messageTextView.setBackgroundColor(color);
  m_textField.setBackgroundColor(color);
}

}  // namespace Probability
