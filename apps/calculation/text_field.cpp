#include "text_field.h"

namespace Calculation {

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, TextFieldDelegate * delegate) :
  ::TextField(parentResponder, textBuffer, textBufferSize, delegate)
{
}

bool TextField::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Ans) {
    KDSize sizePreviousText = KDText::stringSize(m_textBuffer);
    insertTextAtLocation("ans", cursorLocation());
    setCursorLocation(cursorLocation() + strlen("ans"));
    KDSize sizeText = KDText::stringSize(m_textBuffer);
    KDRect dirtyZone(sizePreviousText.width(), 0, sizeText.width()-sizePreviousText.width(), sizeText.height());
    markRectAsDirty(dirtyZone);
    return true;
  }
  if (m_currentTextLength == 0 &&
      (event == Ion::Events::Multiplication ||
      event == Ion::Events::Plus ||
      event == Ion::Events::Minus ||
      event == Ion::Events::Dot ||
      event == Ion::Events::Division)) {
    KDSize sizePreviousText = KDText::stringSize(m_textBuffer);
    insertTextAtLocation("ans", cursorLocation());
    setCursorLocation(cursorLocation() + strlen("ans"));
    KDSize sizeText = KDText::stringSize(m_textBuffer);
    KDRect dirtyZone(sizePreviousText.width(), 0, sizeText.width()-sizePreviousText.width(), sizeText.height());
    markRectAsDirty(dirtyZone);
  }
  return(::TextField::handleEvent(event));
}

}
