#include <escher/menu_cell_with_editable_text.h>

namespace Escher {

AbstractWithEditableText::AbstractWithEditableText(
    Responder* parentResponder, TextFieldDelegate* textFieldDelegate)
    : Responder(parentResponder),
      ChainedTextFieldDelegate(textFieldDelegate),
      m_textField(this, m_textBody, Poincare::PrintFloat::k_maxFloatCharSize,
                  this, k_defaultFormat),
      m_editable(true) {
  m_textBody[0] = 0;
}

// ChainedTextFieldDelegate
void AbstractWithEditableText::textFieldDidStartEditing(
    AbstractTextField* textField) {
  // Relayout to hide sublabel
  relayout();
  ChainedTextFieldDelegate::textFieldDidStartEditing(textField);
}
bool AbstractWithEditableText::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  if (ChainedTextFieldDelegate::textFieldDidFinishEditing(textField, event)) {
    // Relayout to show sublabel
    relayout();
    return true;
  }
  return false;
}

void AbstractWithEditableText::textFieldDidAbortEditing(
    AbstractTextField* textField) {
  // Relayout to show sublabel
  relayout();
  ChainedTextFieldDelegate::textFieldDidAbortEditing(textField);
}
void AbstractWithEditableText::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (m_editable) {
      App::app()->setFirstResponder(&m_textField);
    }
  } else {
    Responder::handleResponderChainEvent(event);
  }
}

}  // namespace Escher
