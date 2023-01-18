#ifndef ESCHER_TEXT_FIELD_DELEGATE_H
#define ESCHER_TEXT_FIELD_DELEGATE_H

#include <ion/events.h>

namespace Escher {
class AbstractTextField;

class TextFieldDelegate {
public:
  virtual bool textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) = 0;
  virtual void textFieldDidStartEditing(AbstractTextField * textField) {}
  virtual bool textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) = 0;
  virtual bool textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) { return false; }
  virtual bool textFieldDidAbortEditing(AbstractTextField * textField) { return false; }
  virtual bool textFieldDidHandleEvent(AbstractTextField * textField, bool returnValue, bool textDidChange) { return returnValue; }
  virtual bool textFieldIsEditable(AbstractTextField * textField) { return true; }
  virtual bool textFieldIsStorable(AbstractTextField * textField) { return true; }
};

}
#endif
