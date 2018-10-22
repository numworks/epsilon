#ifndef ESCHER_TEXT_AREA_DELEGATE_H
#define ESCHER_TEXT_AREA_DELEGATE_H

#include <escher/input_event_handler_delegate.h>

class TextArea;

class TextAreaDelegate : public InputEventHandlerDelegate {
public:
  virtual bool textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) = 0;
};

#endif
