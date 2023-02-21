#ifndef ESCHER_TEXT_AREA_DELEGATE_H
#define ESCHER_TEXT_AREA_DELEGATE_H

namespace Escher {

class TextArea;

class TextAreaDelegate {
 public:
  virtual bool textAreaDidReceiveEvent(TextArea* textArea,
                                       Ion::Events::Event event) = 0;
};

}  // namespace Escher
#endif
