#ifndef ESCHER_INPUT_EVENT_HANDLER_H
#define ESCHER_INPUT_EVENT_HANDLER_H

#include <ion/events.h>

namespace Escher {

// See TODO in EditableField

class InputEventHandler {
 public:
  InputEventHandler() {}
  virtual bool handleEventWithText(const char* text, bool indentation = false,
                                   bool forceCursorRightOfText = false) {
    return false;
  }
  virtual bool handleStoreEvent() { return false; }
  virtual bool handleBoxEvent(Ion::Events::Event event);
};

}  // namespace Escher

#endif
