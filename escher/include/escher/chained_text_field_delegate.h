#ifndef ESCHER_CHAINED_TEXT_FIELD_DELEGATE_H
#define ESCHER_CHAINED_TEXT_FIELD_DELEGATE_H

#include <escher/text_field_delegate.h>

namespace Escher {

/* A ChainedTextFieldDelegates allows to chain TextFieldDelegates so that they can all catch events
 */
class ChainedTextFieldDelegate : public TextFieldDelegate {
public:
  ChainedTextFieldDelegate(TextFieldDelegate * parentDelegate) : m_parentDelegate(parentDelegate) {}
  void setTextFieldDelegate(TextFieldDelegate * delegate) { m_parentDelegate = delegate; }

  bool textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(AbstractTextField * textField) override;
  bool textFieldDidHandleEvent(AbstractTextField * textField, bool returnValue, bool textDidChange) override;
  void textFieldDidStartEditing(AbstractTextField * textField) override;
  bool textFieldIsEditable(AbstractTextField * textField) override;
  bool textFieldIsStorable(AbstractTextField * textField) override;

private:
  TextFieldDelegate * m_parentDelegate;
};

}

#endif
