#ifndef ESCHER_INPUT_EVENT_HANDLER_H
#define ESCHER_INPUT_EVENT_HANDLER_H

#include <ion/events.h>

namespace Escher {

// See TODO in EditableField

class BoxesDelegate;

class InputEventHandler {
 public:
  InputEventHandler(BoxesDelegate* boxesDelegate)
      : m_boxesDelegate(boxesDelegate) {}
  virtual bool handleEventWithText(const char* text, bool indentation = false,
                                   bool forceCursorRightOfText = false) {
    return false;
  }
  virtual bool handleStoreEvent() { return false; }
  bool handleBoxEvent(Ion::Events::Event event);

 protected:
  BoxesDelegate* m_boxesDelegate;
};

}  // namespace Escher

#endif
