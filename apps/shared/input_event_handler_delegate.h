#ifndef SHARED_INPUT_EVENT_HANDLER_DELEGATE_H
#define SHARED_INPUT_EVENT_HANDLER_DELEGATE_H

#include "input_event_handler_delegate_app.h"

namespace Shared {

class InputEventHandlerDelegate : public ::InputEventHandlerDelegate {
public:
  Toolbox * toolboxForInputEventHandler(InputEventHandler * textInput) override { return inputEventHandlerDelegateApp()->toolboxForInputEventHandler(textInput); }
  NestedMenuController * variableBoxForInputEventHandler(InputEventHandler * textInput) override { return inputEventHandlerDelegateApp()->variableBoxForInputEventHandler(textInput); }
private:
  InputEventHandlerDelegateApp * inputEventHandlerDelegateApp() const {
    return static_cast<InputEventHandlerDelegateApp *>(Container::activeApp());
  }
};

}

#endif
