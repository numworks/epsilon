#ifndef ESCHER_TEXT_INPUT_DELEGATE_H
#define ESCHER_TEXT_INPUT_DELEGATE_H

class TextInput;

class TextInputDelegate {
public:
  virtual Toolbox * toolboxForTextInput(TextInput * textInput) = 0;
};

#endif
