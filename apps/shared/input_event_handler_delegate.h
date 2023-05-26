#ifndef SHARED_INPUT_EVENT_HANDLER_DELEGATE_H
#define SHARED_INPUT_EVENT_HANDLER_DELEGATE_H

#include <escher/container.h>

#include "shared_app.h"

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
  SharedApp *inputEventHandlerDelegateApp() const {
    return static_cast<SharedApp *>(Escher::Container::activeApp());
  }
};

}  // namespace Shared

#endif
