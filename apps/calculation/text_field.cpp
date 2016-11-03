#include "text_field.h"

namespace Calculation {

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, TextFieldDelegate * delegate) :
  ::TextField(parentResponder, textBuffer, textBufferSize, delegate)
{
}

bool TextField::handleEvent(Ion::Events::Event event) {
  if (m_currentTextLength == 0 &&
      (event == Ion::Events::Event::PRODUCT ||
      event == Ion::Events::Event::PLUS ||
      event == Ion::Events::Event::MINUS ||
      event == Ion::Events::Event::DOT ||
      event == Ion::Events::Event::DIVISION)) {
    KDSize sizePreviousText = KDText::stringSize(m_textBuffer);
    m_textBuffer[m_currentTextLength++] = 'a';
    m_textBuffer[m_currentTextLength++] = 'n';
    m_textBuffer[m_currentTextLength++] = 's';
    m_textBuffer[m_currentTextLength] = 0;
    KDSize sizeText = KDText::stringSize(m_textBuffer);
    KDRect dirtyZone(sizePreviousText.width(), 0, sizeText.width()-sizePreviousText.width(), sizeText.height());
    markRectAsDirty(dirtyZone);
  }
  return(::TextField::handleEvent(event));
}

}
