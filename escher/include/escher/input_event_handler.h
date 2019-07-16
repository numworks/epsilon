#ifndef ESCHER_INPUT_EVENT_HANDLER_H
#define ESCHER_INPUT_EVENT_HANDLER_H

#include <ion/events.h>

class InputEventHandlerDelegate;

class InputEventHandler {
public:
  InputEventHandler(InputEventHandlerDelegate * inputEventHandlerdelegate) : m_inputEventHandlerDelegate(inputEventHandlerdelegate) {}
  virtual bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) { return false; }
protected:
  bool handleBoxEvent(Ion::Events::Event event);
  InputEventHandlerDelegate * m_inputEventHandlerDelegate;
};

#endif
