#include <escher/text_field.h>

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, TextFieldDelegate * delegate) :
  View(),
  Responder(parentResponder),
  m_textBuffer(textBuffer),
  m_currentTextLength(0),
  m_currentCursorPosition(0),
  m_textBufferSize(textBufferSize),
  m_delegate(delegate)
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
  if (m_delegate) {
    if (m_delegate->textFieldDidReceiveEvent(this, event)) {
      return true;
    }
  }
  switch (event) {
    case Ion::Events::Event::LEFT_ARROW:
      if (m_currentCursorPosition > 0) {
        m_currentCursorPosition--;
      }
      return true;
    case Ion::Events::Event::RIGHT_ARROW:
      if (m_currentCursorPosition < m_currentTextLength) {
        m_currentCursorPosition++;
      }
      return true;
    case Ion::Events::Event::DELETE:
      if (m_currentCursorPosition > 0) {
        KDSize sizePreviousText = KDText::stringSize(m_textBuffer);
        m_currentTextLength--;
        m_currentCursorPosition--;
        for (int k = m_currentCursorPosition; k < m_currentTextLength; k ++) {
          m_textBuffer[k] = m_textBuffer[k+1];
        }
        m_textBuffer[m_currentTextLength] = 0;
        KDRect dirtyZone(0, 0, sizePreviousText.width(), sizePreviousText.height());
        markRectAsDirty(dirtyZone);
      }
      return true;
    default:
      if ((int)event >= 0x100) {
        return false;
      }
      if (m_currentTextLength == 0 || m_currentTextLength-1 < m_textBufferSize) {
        for (int k = m_currentTextLength; k > m_currentCursorPosition; k--) {
          m_textBuffer[k] = m_textBuffer[k-1];
        }
        m_textBuffer[++m_currentTextLength] = 0;
        m_textBuffer[m_currentCursorPosition++] = (int)event;
        KDSize sizeText = KDText::stringSize(m_textBuffer);
        KDRect dirtyZone(0, 0, sizeText.width(), sizeText.height());
        markRectAsDirty(dirtyZone);
      }
      return true;
  }
}

const char * TextField::textBuffer () const {
  const char * textBuffer = (const char *)m_textBuffer;
  return textBuffer;
}

int TextField::bufferLength () const {
  return m_currentTextLength;
}

void TextField::setTextBuffer(const char * text) {
  strlcpy(m_textBuffer, text, m_textBufferSize);
  m_currentCursorPosition = strlen(text);
  m_currentTextLength = m_currentCursorPosition;
  markRectAsDirty(bounds());
}

KDSize TextField::minimalSizeForOptimalDisplay() {
  KDSize textSize = KDText::stringSize(m_textBuffer);
  return KDSize(0, textSize.height());
}
