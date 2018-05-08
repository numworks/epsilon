#ifndef ESCHER_TEXT_FIELD_DELEGATE_H
#define ESCHER_TEXT_FIELD_DELEGATE_H

#include <escher/text_input_delegate.h>

class TextField;

class TextFieldDelegate : public TextInputDelegate {
public:
  virtual bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) = 0;
  virtual bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) = 0;
  virtual bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) { return false; };
  virtual bool textFieldDidAbortEditing(TextField * textField, const char * text) {return false;};
  virtual bool textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textHasChanged) { return returnValue; };
};

#endif
