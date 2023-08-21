#include <escher/chained_text_field_delegate.h>

namespace Escher {

bool ChainedTextFieldDelegate::textFieldShouldFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  return m_parentDelegate
             ? m_parentDelegate->textFieldShouldFinishEditing(textField, event)
             : false;
}

bool ChainedTextFieldDelegate::textFieldDidReceiveEvent(
    AbstractTextField* textField, Ion::Events::Event event) {
  return m_parentDelegate
             ? m_parentDelegate->textFieldDidReceiveEvent(textField, event)
             : false;
}

bool ChainedTextFieldDelegate::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  return m_parentDelegate
             ? m_parentDelegate->textFieldDidFinishEditing(textField, event)
             : false;
}

void ChainedTextFieldDelegate::textFieldDidAbortEditing(
    AbstractTextField* textField) {
  if (m_parentDelegate) {
    m_parentDelegate->textFieldDidAbortEditing(textField);
  }
}

void ChainedTextFieldDelegate::textFieldDidHandleEvent(
    AbstractTextField* textField) {
  if (m_parentDelegate) {
    m_parentDelegate->textFieldDidHandleEvent(textField);
  }
}

void ChainedTextFieldDelegate::textFieldDidStartEditing(
    AbstractTextField* textField) {
  if (m_parentDelegate) {
    m_parentDelegate->textFieldDidStartEditing(textField);
  }
}

bool ChainedTextFieldDelegate::textFieldIsEditable(
    AbstractTextField* textField) {
  return m_parentDelegate ? m_parentDelegate->textFieldIsEditable(textField)
                          : TextFieldDelegate::textFieldIsEditable(textField);
}

bool ChainedTextFieldDelegate::textFieldIsStorable(
    AbstractTextField* textField) {
  return m_parentDelegate ? m_parentDelegate->textFieldIsStorable(textField)
                          : TextFieldDelegate::textFieldIsStorable(textField);
}

}  // namespace Escher
