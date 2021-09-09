#include <escher/chained_text_field_delegate.h>

namespace Escher {

bool ChainedTextFieldDelegate::textFieldShouldFinishEditing(TextField * textField,
                                                            Ion::Events::Event event) {
  return m_parentDelegate ? m_parentDelegate->textFieldShouldFinishEditing(textField, event)
                          : false;
}

bool ChainedTextFieldDelegate::textFieldDidReceiveEvent(TextField * textField,
                                                        Ion::Events::Event event) {
  return m_parentDelegate ? m_parentDelegate->textFieldDidReceiveEvent(textField, event) : false;
}

bool ChainedTextFieldDelegate::textFieldDidFinishEditing(TextField * textField,
                                                         const char * text,
                                                         Ion::Events::Event event) {
  return m_parentDelegate ? m_parentDelegate->textFieldDidFinishEditing(textField, text, event)
                          : false;
}

bool ChainedTextFieldDelegate::textFieldDidAbortEditing(TextField * textField) {
  return m_parentDelegate ? m_parentDelegate->textFieldDidAbortEditing(textField) : false;
}

bool ChainedTextFieldDelegate::textFieldDidHandleEvent(TextField * textField,
                                                       bool returnValue,
                                                       bool textSizeDidChange) {
  return m_parentDelegate
             ? m_parentDelegate->textFieldDidHandleEvent(textField, returnValue, textSizeDidChange)
             : false;
}

void ChainedTextFieldDelegate::textFieldDidStartEditing(TextField * textField) {
  if (m_parentDelegate) {
    m_parentDelegate->textFieldDidStartEditing(textField);
  }
}

}  // namespace Escher
