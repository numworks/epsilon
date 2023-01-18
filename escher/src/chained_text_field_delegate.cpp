#include <escher/chained_text_field_delegate.h>

namespace Escher {

bool ChainedTextFieldDelegate::textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) {
  return m_parentDelegate ? m_parentDelegate->textFieldShouldFinishEditing(textField, event) : false;
}

bool ChainedTextFieldDelegate::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  return m_parentDelegate ? m_parentDelegate->textFieldDidReceiveEvent(textField, event) : false;
}

bool ChainedTextFieldDelegate::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  return m_parentDelegate ? m_parentDelegate->textFieldDidFinishEditing(textField, text, event) : false;
}

bool ChainedTextFieldDelegate::textFieldDidAbortEditing(AbstractTextField * textField) {
  return m_parentDelegate ? m_parentDelegate->textFieldDidAbortEditing(textField) : false;
}

bool ChainedTextFieldDelegate::textFieldDidHandleEvent(AbstractTextField * textField, bool returnValue, bool textDidChange) {
  return m_parentDelegate ? m_parentDelegate->textFieldDidHandleEvent(textField, returnValue, textDidChange) : false;
}

void ChainedTextFieldDelegate::textFieldDidStartEditing(AbstractTextField * textField) {
  if (m_parentDelegate) {
    m_parentDelegate->textFieldDidStartEditing(textField);
  }
}

bool ChainedTextFieldDelegate::textFieldIsEditable(AbstractTextField * textField) {
  return m_parentDelegate ? m_parentDelegate->textFieldIsEditable(textField) : TextFieldDelegate::textFieldIsEditable(textField);
}

bool ChainedTextFieldDelegate::textFieldIsStorable(AbstractTextField * textField) {
  return m_parentDelegate ? m_parentDelegate->textFieldIsStorable(textField) : TextFieldDelegate::textFieldIsStorable(textField);
}

}
