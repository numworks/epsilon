#ifndef SHARED_INPUT_EVENT_HANDLER_DELEGATE_APP_H
#define SHARED_INPUT_EVENT_HANDLER_DELEGATE_APP_H

#include "store_menu_controller.h"
#include <escher/app.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/nested_menu_controller.h>

namespace Shared {

class InputEventHandlerDelegateApp : public Escher::App, public Escher::InputEventHandlerDelegate {
public:
  virtual ~InputEventHandlerDelegateApp() = default;
  Escher::NestedMenuController * toolbox() override;
  Escher::NestedMenuController * variableBox() override;

protected:
  bool isVarBoxMenuOpen();
  InputEventHandlerDelegateApp(Snapshot * snapshot, Escher::ViewController * rootViewController);
};

}

#endif
