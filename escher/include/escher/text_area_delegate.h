#ifndef ESCHER_TEXT_AREA_DELEGATE_H
#define ESCHER_TEXT_AREA_DELEGATE_H

class TextArea;

class TextAreaDelegate {
public:
  virtual void textAreaDidReceiveNoneXNTEvent() {};
  virtual bool textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) = 0;
};

#endif
