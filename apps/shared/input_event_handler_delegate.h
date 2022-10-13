#ifndef SHARED_INPUT_EVENT_HANDLER_DELEGATE_H
#define SHARED_INPUT_EVENT_HANDLER_DELEGATE_H

#include "input_event_handler_delegate_app.h"
#include <escher/container.h>

namespace Shared {

class InputEventHandlerDelegate : public Escher::InputEventHandlerDelegate {
public:
  Escher::Toolbox * toolboxForInputEventHandler() override { return inputEventHandlerDelegateApp()->toolboxForInputEventHandler(); }
  Escher::NestedMenuController * variableBoxForInputEventHandler() override { return inputEventHandlerDelegateApp()->variableBoxForInputEventHandler(); }
private:
  InputEventHandlerDelegateApp * inputEventHandlerDelegateApp() const {
    return static_cast<InputEventHandlerDelegateApp *>(Escher::Container::activeApp());
  }
};

}

#endif
