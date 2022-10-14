#ifndef SHARED_INPUT_EVENT_HANDLER_DELEGATE_APP_H
#define SHARED_INPUT_EVENT_HANDLER_DELEGATE_APP_H

#include "store_menu_controller.h"
#include <escher/app.h>
#include <escher/input_event_handler_delegate.h>

namespace Shared {

class InputEventHandlerDelegateApp : public Escher::App, public Escher::InputEventHandlerDelegate {
public:
  virtual ~InputEventHandlerDelegateApp() = default;
  Escher::PervasiveBox * toolbox() override;
  Escher::PervasiveBox * variableBox() override;

protected:
  InputEventHandlerDelegateApp(Snapshot * snapshot, Escher::ViewController * rootViewController);
};

}

#endif
