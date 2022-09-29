#ifndef SHARED_INPUT_EVENT_HANDLER_DELEGATE_APP_H
#define SHARED_INPUT_EVENT_HANDLER_DELEGATE_APP_H

#include <escher/app.h>
#include <escher/input_event_handler_delegate.h>

namespace Shared {

class InputEventHandlerDelegateApp : public Escher::App, public Escher::InputEventHandlerDelegate {
public:
  virtual ~InputEventHandlerDelegateApp() = default;
  Escher::Toolbox * toolboxForInputEventHandler(Escher::InputEventHandler * textInput) override;
  Escher::NestedMenuController * variableBoxForInputEventHandler(Escher::InputEventHandler * textInput) override;
  bool handleEvent(Ion::Events::Event event) override;
protected:
  InputEventHandlerDelegateApp(Snapshot * snapshot, Escher::ViewController * rootViewController);
};

}

#endif
