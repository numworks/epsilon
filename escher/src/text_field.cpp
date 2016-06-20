#include <escher/text_field.h>

TextField::TextField(char * textBuffer, size_t textBufferSize) :
  ChildlessView(),
  m_textBuffer(textBuffer),
  m_textBufferSize(textBufferSize),
  m_currentTextLength(0)
{
}

/* View */

void TextField::drawRect(KDRect rect) const {
  m_textBuffer[m_currentTextLength] = 0;
  KDDrawString(m_textBuffer, KDPointZero, 0);
}

#if ESCHER_VIEW_LOGGING
const char * TextField::className() const {
  return "TextField";
}
#endif

/* Responder */

bool TextField::handleEvent(ion_event_t event) {
  if (event == DELETE && m_currentTextLength > 0) {
    m_currentTextLength--;
    return true;
  }
  if (event >= 0x100) {
    return false;
  }
  if (m_currentTextLength-1 < m_textBufferSize) {
    m_textBuffer[m_currentTextLength++] = event;
    markRectAsDirty(bounds()); // TODO: Could be optimized
  }
  return true;
}
