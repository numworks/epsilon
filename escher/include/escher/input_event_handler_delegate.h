#ifndef ESCHER_INPUT_EVENT_HANDLER_DELEGATE_H
#define ESCHER_INPUT_EVENT_HANDLER_DELEGATE_H

#include "pervasive_box.h"

namespace Escher {

class InputEventHandlerDelegate {
 public:
  virtual PervasiveBox* toolbox() = 0;
  virtual PervasiveBox* variableBox() = 0;
};

}  // namespace Escher

#endif
