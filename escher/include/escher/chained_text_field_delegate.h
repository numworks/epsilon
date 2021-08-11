#ifndef ESCHER_INCLUDE_ESCHER_CHAINED_TEXT_FIELD_DELEGATE_H
#define ESCHER_INCLUDE_ESCHER_CHAINED_TEXT_FIELD_DELEGATE_H

#include <escher/text_field_delegate.h>

namespace Escher {

/* A ChainedTextFieldDelegates allows to chain TextFieldDelegates so that they can all catch events
 */
class ChainedTextFieldDelegate : public TextFieldDelegate {
public:
  ChainedTextFieldDelegate(TextFieldDelegate * nextDelegate) : m_nextDelegate(nextDelegate) {}
  void setTextFieldDelegate(TextFieldDelegate * delegate) { m_nextDelegate = delegate; }

  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;
  bool textFieldDidHandleEvent(TextField * textField,
                               bool returnValue,
                               bool textSizeDidChange) override;
  void textFieldDidStartEditing(TextField * textField) override;

private:
  TextFieldDelegate * m_nextDelegate;
};

}  // namespace Escher

#endif /* ESCHER_INCLUDE_ESCHER_CHAINED_TEXT_FIELD_DELEGATE_H */
