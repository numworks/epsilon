#ifndef ESCHER_TEXT_FIELD_DELEGATE_H
#define ESCHER_TEXT_FIELD_DELEGATE_H

class TextField;

class TextFieldDelegate {
public:
  virtual bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) = 0;
};

#endif
