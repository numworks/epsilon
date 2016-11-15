#include <escher/text_field.h>
#include <assert.h>

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

void TextField::reload() {
  KDSize sizeText = KDText::stringSize(m_textBuffer);
  KDRect dirtyZone(0, 0, sizeText.width(), sizeText.height());
  markRectAsDirty(dirtyZone);
}

/* Responder */

bool TextField::handleEvent(Ion::Events::Event event) {
  if (m_delegate) {
    if (m_delegate->textFieldDidReceiveEvent(this, event)) {
      return true;
    }
  }
  if (event == Ion::Events::Left) {
    if (m_currentCursorPosition > 0) {
      m_currentCursorPosition--;
    }
    return true;
  }
  if (event == Ion::Events::Right) {
    if (m_currentCursorPosition < m_currentTextLength) {
      m_currentCursorPosition++;
    }
    return true;
  }
  if (event == Ion::Events::Backspace) {
    if (m_currentCursorPosition > 0) {
      reload();
      m_currentTextLength--;
      m_currentCursorPosition--;
      for (int k = m_currentCursorPosition; k < m_currentTextLength; k ++) {
      m_textBuffer[k] = m_textBuffer[k+1];
      }
      m_textBuffer[m_currentTextLength] = 0;
    }
    return true;
  }
  if (event.hasText()) {
    // FIXME: Only inserting the first letter!
    if (m_currentTextLength == 0 || m_currentTextLength-1 < m_textBufferSize) {
      for (int k = m_currentTextLength; k > m_currentCursorPosition; k--) {
        m_textBuffer[k] = m_textBuffer[k-1];
      }
      m_textBuffer[++m_currentTextLength] = 0;
      m_textBuffer[m_currentCursorPosition++] = event.text()[0];
      reload();
    }
    return true;
  }
  return false;
}

const char * TextField::text() const {
  return (const char *)m_textBuffer;
}

int TextField::textLength() const {
  assert(strlen(text()) == m_currentTextLength);
  return m_currentTextLength;
}

void TextField::setText(const char * text) {
  strlcpy(m_textBuffer, text, m_textBufferSize);
  m_currentCursorPosition = strlen(text);
  m_currentTextLength = m_currentCursorPosition;
  reload();
}

void TextField::appendText(const char * text) {
  int textSize = strlen(text);
  if (m_currentTextLength + textSize > m_textBufferSize) {
    return;
  }
  for (int k = m_currentTextLength; k >= m_currentCursorPosition && k >= 0; k--) {
    m_textBuffer[k+textSize] = m_textBuffer[k];
  }
  strlcpy(&m_textBuffer[m_currentCursorPosition], text, textSize);
  m_currentCursorPosition += textSize;
  m_textBuffer[m_currentCursorPosition-1] = text[textSize-1];
  m_currentTextLength += textSize;
  reload();
}

void TextField::moveCursor(int position) {
  assert(m_currentCursorPosition + position <= m_currentTextLength);
  assert(m_currentCursorPosition + position >= 0);
  m_currentCursorPosition += position;
}

KDSize TextField::minimalSizeForOptimalDisplay() {
  KDSize textSize = KDText::stringSize(m_textBuffer);
  return KDSize(0, textSize.height());
}
