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

}  // namespace Probability
