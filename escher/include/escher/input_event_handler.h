#ifndef ESCHER_INPUT_EVENT_HANDLER_H
#define ESCHER_INPUT_EVENT_HANDLER_H

class Toolbox;
class App;

class InputEventHandler {
public:
  virtual bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) { return false; }
  virtual Toolbox * toolbox() { return nullptr; }
protected:
  bool handleToolboxEvent(App * app);
};

#endif
