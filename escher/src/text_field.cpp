#include <escher/text_field.h>

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize) :
  Responder(parentResponder),
  ChildlessView(),
  m_textBuffer(textBuffer),
  m_textBufferSize(textBufferSize),
  m_currentTextLength(0)
{
}

/* View */

void TextField::drawRect(KDContext * ctx, KDRect rect) const {
  m_textBuffer[m_currentTextLength] = 0;
  ctx->drawString(m_textBuffer, KDPointZero);
}

#if ESCHER_VIEW_LOGGING
const char * TextField::className() const {
  return "TextField";
}
#endif

/* Responder */

bool TextField::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Event::DELETE && m_currentTextLength > 0) {
    m_currentTextLength--;
    return true;
  }
  if ((int)event >= 0x100) {
    return false;
  }
  if (m_currentTextLength-1 < m_textBufferSize) {
    m_textBuffer[m_currentTextLength++] = (int)event;
    markRectAsDirty(bounds()); // TODO: Could be optimized
  }
  return true;
}
