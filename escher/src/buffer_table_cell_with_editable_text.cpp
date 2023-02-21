#include <escher/buffer_table_cell_with_editable_text.h>
#include <escher/palette.h>

namespace Escher {

BufferTableCellWithEditableText::BufferTableCellWithEditableText(
    Responder* parentResponder,
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* textFieldDelegate)
    : Responder(parentResponder),
      BufferTableCell(),
      m_textField(this, m_textBody, Poincare::PrintFloat::k_maxFloatCharSize,
                  TextField::MaxBufferSize(), inputEventHandlerDelegate,
                  textFieldDelegate, KDFont::Size::Large,
                  KDContext::k_alignRight, KDContext::k_alignCenter,
                  KDColorBlack),
      m_editable(true) {
  m_textBody[0] = '\0';
}

void BufferTableCellWithEditableText::setHighlighted(bool highlight) {
  BufferTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  m_textField.setBackgroundColor(backgroundColor);
}

void BufferTableCellWithEditableText::setAccessoryText(const char* text) {
  m_textField.setText(text);
  layoutSubviews();
}

void BufferTableCellWithEditableText::setDelegates(
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* textFieldDelegate) {
  m_textField.setDelegates(inputEventHandlerDelegate, textFieldDelegate);
}

}  // namespace Escher
