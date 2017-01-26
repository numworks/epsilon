#include "text_field.h"

namespace Calculation {

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, TextFieldDelegate * delegate) :
  ::TextField(parentResponder, textBuffer, textBuffer, textBufferSize, delegate)
{
  m_isEditing = true;
}

bool TextField::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    return false;
  }
  if (event == Ion::Events::Ans) {
    if (!isEditing()) {
      setEditing(true);
      setText("");
    }
    insertTextAtLocation("ans", cursorLocation());
    setCursorLocation(cursorLocation() + strlen("ans"));
    return true;
  }
  if (m_currentTextLength == 0 &&
      (event == Ion::Events::Multiplication ||
      event == Ion::Events::Plus ||
      event == Ion::Events::Dot ||
      event == Ion::Events::Division)) {
    if (!isEditing()) {
      setEditing(true);
      setText("");
    }
    insertTextAtLocation("ans", cursorLocation());
    setCursorLocation(cursorLocation() + strlen("ans"));
  }
  return(::TextField::handleEvent(event));
}

void TextField::setEditing(bool isEditing) {
  m_isEditing = true;
}

}
