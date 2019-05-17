#ifndef SHARED_INPUT_EVENT_HANDLER_DELEGATE_APP_H
#define SHARED_INPUT_EVENT_HANDLER_DELEGATE_APP_H

#include <escher.h>

namespace Shared {

class InputEventHandlerDelegateApp : public ::App, public InputEventHandlerDelegate {
public:
  virtual ~InputEventHandlerDelegateApp() = default;
  Toolbox * toolboxForInputEventHandler(InputEventHandler * textInput) override;
  NestedMenuController * variableBoxForInputEventHandler(InputEventHandler * textInput) override;
protected:
  InputEventHandlerDelegateApp(Snapshot * snapshot, ViewController * rootViewController);
};

}

#endif
