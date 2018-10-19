#ifndef ESCHER_TEXT_INPUT_DELEGATE_H
#define ESCHER_TEXT_INPUT_DELEGATE_H

#include <escher/input_event_handler.h>
class TextInput;

class TextInputDelegate {
public:
  virtual Toolbox * toolboxForTextInput(InputEventHandler * textInput) = 0;
};

#endif
