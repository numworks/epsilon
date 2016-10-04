#include <escher/text_field.h>

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize) :
  ChildlessView(),
  Responder(parentResponder),
  m_textBuffer(textBuffer),
  m_textBufferSize(textBufferSize),
  m_currentTextLength(0)
{
}

/* View */

void TextField::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
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
    KDSize sizePreviousText = KDText::stringSize(m_textBuffer);
    m_currentTextLength--;
    m_textBuffer[m_currentTextLength] = 0;
    KDSize sizeText = KDText::stringSize(m_textBuffer);
    KDRect dirtyZone(sizeText.width(), 0, sizePreviousText.width()-sizeText.width(), sizeText.height());
    markRectAsDirty(dirtyZone);
    return true;
  }
  if ((int)event >= 0x100) {
    return false;
  }
  if (m_currentTextLength == 0 || m_currentTextLength-1 < m_textBufferSize) {
    KDSize sizePreviousText = KDText::stringSize(m_textBuffer);
    m_textBuffer[m_currentTextLength++] = (int)event;
    m_textBuffer[m_currentTextLength] = 0;
    KDSize sizeText = KDText::stringSize(m_textBuffer);
    KDRect dirtyZone(sizePreviousText.width(), 0, sizeText.width()-sizePreviousText.width(), sizeText.height());
    markRectAsDirty(dirtyZone);
  }
  return true;
}

const char * TextField::textBuffer () const {
  const char * textBuffer = (const char *)m_textBuffer;
  return textBuffer;
}

void TextField::setTextBuffer(const char * text) {
  strlcpy(m_textBuffer, text, m_textBufferSize);
  m_currentTextLength = strlen(text);
}
