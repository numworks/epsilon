#ifndef ESCHER_INPUT_EVENT_HANDLER_H
#define ESCHER_INPUT_EVENT_HANDLER_H

class InputEventHandlerDelegate;
class App;

class InputEventHandler {
public:
  virtual bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) { return false; }
protected:
  bool handleToolboxEvent(App * app);
private:
  virtual InputEventHandlerDelegate * inputEventHandlerDelegate() { return nullptr; };
};

#endif
