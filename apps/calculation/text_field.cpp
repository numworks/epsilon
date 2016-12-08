#include "text_field.h"

namespace Calculation {

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, TextFieldDelegate * delegate) :
  ::TextField(parentResponder, textBuffer, textBufferSize, delegate)
{
}

bool TextField::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Ans) {
    insertTextAtLocation("ans", cursorLocation());
    setCursorLocation(cursorLocation() + strlen("ans"));
    return true;
  }
  if (m_currentTextLength == 0 &&
      (event == Ion::Events::Multiplication ||
      event == Ion::Events::Plus ||
      event == Ion::Events::Minus ||
      event == Ion::Events::Dot ||
      event == Ion::Events::Division)) {
    insertTextAtLocation("ans", cursorLocation());
    setCursorLocation(cursorLocation() + strlen("ans"));
  }
  return(::TextField::handleEvent(event));
}

}
