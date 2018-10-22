#ifndef ESCHER_TEXT_FIELD_DELEGATE_H
#define ESCHER_TEXT_FIELD_DELEGATE_H

#include <escher/input_event_handler_delegate.h>

class TextField;

class TextFieldDelegate : public InputEventHandlerDelegate {
public:
  virtual bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) = 0;
  virtual bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) = 0;
  virtual bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) { return false; };
  virtual bool textFieldDidAbortEditing(TextField * textField) {return false;};
  virtual bool textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textHasChanged) { return returnValue; };
};

#endif
