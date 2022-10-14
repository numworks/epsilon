#ifndef ESCHER_INPUT_EVENT_HANDLER_H
#define ESCHER_INPUT_EVENT_HANDLER_H

#include <ion/events.h>

namespace Escher {

// See TODO in EditableField

class InputEventHandlerDelegate;

class InputEventHandler {
public:
  InputEventHandler(InputEventHandlerDelegate * inputEventHandlerdelegate) : m_inputEventHandlerDelegate(inputEventHandlerdelegate) {}
  virtual bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) { return false; }
  virtual bool handleStoreEvent() { return false; }
  bool handleBoxEvent(Ion::Events::Event event);
protected:
  InputEventHandlerDelegate * m_inputEventHandlerDelegate;
};

}

#endif
