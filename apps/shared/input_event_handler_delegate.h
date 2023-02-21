#ifndef SHARED_INPUT_EVENT_HANDLER_DELEGATE_H
#define SHARED_INPUT_EVENT_HANDLER_DELEGATE_H

#include <escher/container.h>

#include "input_event_handler_delegate_app.h"

namespace Shared {

class InputEventHandlerDelegate : public Escher::InputEventHandlerDelegate {
 public:
  Escher::PervasiveBox *toolbox() override {
    return inputEventHandlerDelegateApp()->toolbox();
  }
  Escher::PervasiveBox *variableBox() override {
    return inputEventHandlerDelegateApp()->variableBox();
  }

 private:
  InputEventHandlerDelegateApp *inputEventHandlerDelegateApp() const {
    return static_cast<InputEventHandlerDelegateApp *>(
        Escher::Container::activeApp());
  }
};

}  // namespace Shared

#endif
