#ifndef ESCHER_TEXT_AREA_DELEGATE_H
#define ESCHER_TEXT_AREA_DELEGATE_H

class TextArea;

class TextAreaDelegate {
public:
  virtual bool textAreaShouldFinishEditing(TextArea * textArea, Ion::Events::Event event) = 0;
  virtual bool textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) = 0;
  virtual Toolbox * toolboxForTextArea(TextArea * textFied) = 0;
};

#endif
