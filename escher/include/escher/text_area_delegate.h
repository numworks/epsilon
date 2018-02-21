#ifndef ESCHER_TEXT_AREA_DELEGATE_H
#define ESCHER_TEXT_AREA_DELEGATE_H

#include <escher/text_input_delegate.h>

class TextArea;

class TextAreaDelegate : public TextInputDelegate {
public:
  virtual bool textAreaShouldFinishEditing(TextArea * textArea, Ion::Events::Event event) = 0;
  virtual bool textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) = 0;
};

#endif
