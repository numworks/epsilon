#include <escher/menu_cell_with_editable_text.h>

namespace Escher {

AbstractWithEditableText::AbstractWithEditableText(
    Responder* parentResponder,
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* textFieldDelegate)
    : Responder(parentResponder),
      ChainedTextFieldDelegate(textFieldDelegate),
      m_textField(this, m_textBody, Poincare::PrintFloat::k_maxFloatCharSize,
                  inputEventHandlerDelegate, this, KDFont::Size::Large,
                  KDGlyph::k_alignRight, KDGlyph::k_alignCenter, KDColorBlack),
      m_editable(true) {
  m_textBody[0] = '\0';
}

void AbstractWithEditableText::setDelegates(
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* textFieldDelegate) {
  m_textField.setInputEventHandlerDelegate(inputEventHandlerDelegate);
  ChainedTextFieldDelegate::setTextFieldDelegate(textFieldDelegate);
}

// ChainedTextFieldDelegate
void AbstractWithEditableText::textFieldDidStartEditing(
    AbstractTextField* textField) {
  // Relayout to hide sublabel
  relayout();
  ChainedTextFieldDelegate::textFieldDidStartEditing(textField);
}
bool AbstractWithEditableText::textFieldDidFinishEditing(
    AbstractTextField* textField, const char* text, Ion::Events::Event event) {
  if (ChainedTextFieldDelegate::textFieldDidFinishEditing(textField, text,
                                                          event)) {
    // Relayout to show sublabel
    relayout();
    return true;
  }
  return false;
}

bool AbstractWithEditableText::textFieldDidAbortEditing(
    AbstractTextField* textField) {
  // Relayout to show sublabel
  relayout();
  return ChainedTextFieldDelegate::textFieldDidAbortEditing(textField);
}

}  // namespace Escher
