#ifndef ESCHER_INPUT_EVENT_HANDLER_DELEGATE_H
#define ESCHER_INPUT_EVENT_HANDLER_DELEGATE_H

namespace Escher {

class InputEventHandler;
class Toolbox;
class NestedMenuController;

class InputEventHandlerDelegate {
public:
  virtual Toolbox * toolboxForInputEventHandler() = 0;
  virtual NestedMenuController * variableBoxForInputEventHandler() = 0;
};

}

#endif
