#ifndef ESCHER_CHAINED_TEXT_FIELD_DELEGATE_H
#define ESCHER_CHAINED_TEXT_FIELD_DELEGATE_H

#include <escher/text_field_delegate.h>

namespace Escher {

/* A ChainedTextFieldDelegates allows to chain TextFieldDelegates so that they
 * can all catch events
 */
class ChainedTextFieldDelegate : public TextFieldDelegate {
 public:
  ChainedTextFieldDelegate(TextFieldDelegate* parentDelegate)
      : m_parentDelegate(parentDelegate) {}
  void setDelegate(TextFieldDelegate* delegate) { m_parentDelegate = delegate; }

  bool textFieldShouldFinishEditing(AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(AbstractTextField* textField,
                                Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(AbstractTextField* textField,
                                 Ion::Events::Event event) override;
  void textFieldDidAbortEditing(AbstractTextField* textField) override;
  void textFieldDidHandleEvent(AbstractTextField* textField) override;
  void textFieldDidStartEditing(AbstractTextField* textField) override;
  bool textFieldIsEditable(AbstractTextField* textField) override;
  bool textFieldIsStorable(AbstractTextField* textField) override;

 private:
  TextFieldDelegate* m_parentDelegate;
};

}  // namespace Escher

#endif
