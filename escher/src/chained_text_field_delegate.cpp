#include <escher/chained_text_field_delegate.h>

namespace Escher {

bool ChainedTextFieldDelegate::textFieldShouldFinishEditing(TextField * textField,
                                                            Ion::Events::Event event) {
  return m_nextDelegate ? m_nextDelegate->textFieldShouldFinishEditing(textField, event) : false;
}

bool ChainedTextFieldDelegate::textFieldDidReceiveEvent(TextField * textField,
                                                        Ion::Events::Event event) {
  return m_nextDelegate ? m_nextDelegate->textFieldDidReceiveEvent(textField, event) : false;
}

bool ChainedTextFieldDelegate::textFieldDidFinishEditing(TextField * textField,
                                                         const char * text,
                                                         Ion::Events::Event event) {
  return m_nextDelegate ? m_nextDelegate->textFieldDidFinishEditing(textField, text, event) : false;
}

bool ChainedTextFieldDelegate::textFieldDidAbortEditing(TextField * textField) {
  return m_nextDelegate ? m_nextDelegate->textFieldDidAbortEditing(textField) : false;
}

bool ChainedTextFieldDelegate::textFieldDidHandleEvent(TextField * textField,
                                                       bool returnValue,
                                                       bool textSizeDidChange) {
  return m_nextDelegate
             ? m_nextDelegate->textFieldDidHandleEvent(textField, returnValue, textSizeDidChange)
             : false;
}

void ChainedTextFieldDelegate::textFieldDidStartEditing(TextField * textField) {
  if (m_nextDelegate) {
    m_nextDelegate->textFieldDidStartEditing(textField);
  }
}

}  // namespace Escher
