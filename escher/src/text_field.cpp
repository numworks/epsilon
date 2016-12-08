#include <escher/text_field.h>
#include <assert.h>

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, TextFieldDelegate * delegate) :
  View(),
  Responder(parentResponder),
  m_textBuffer(textBuffer),
  m_currentTextLength(0),
  m_currentCursorLocation(0),
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
    if (m_currentCursorLocation > 0) {
      m_currentCursorLocation--;
    }
    return true;
  }
  if (event == Ion::Events::Right) {
    if (m_currentCursorLocation < m_currentTextLength) {
      m_currentCursorLocation++;
    }
    return true;
  }
  if (event == Ion::Events::Backspace) {
    if (m_currentCursorLocation > 0) {
      reload();
      m_currentTextLength--;
      m_currentCursorLocation--;
      for (int k = m_currentCursorLocation; k < m_currentTextLength; k ++) {
      m_textBuffer[k] = m_textBuffer[k+1];
      }
      m_textBuffer[m_currentTextLength] = 0;
    }
    return true;
  }
  if (event.hasText()) {
    insertTextAtLocation(event.text(), cursorLocation());
    int cursorDelta = strlen(event.text()) > 1 ? -1 : 0;
    setCursorLocation(cursorLocation() + strlen(event.text()) + cursorDelta);
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
  reload();
  strlcpy(m_textBuffer, text, m_textBufferSize);
  m_currentCursorLocation = strlen(text);
  m_currentTextLength = m_currentCursorLocation;
  reload();
}

int TextField::cursorLocation() const{
  return m_currentCursorLocation;
}

void TextField::setCursorLocation(int location) {
  location = location < 0 ? 0 : location;
  location = location > m_currentTextLength ? m_currentTextLength : location;
  m_currentCursorLocation = location;
}

void TextField::insertTextAtLocation(const char * text, int location) {
  int textSize = strlen(text);
  if (m_currentTextLength + textSize > m_textBufferSize) {
    return;
  }
  for (int k = m_currentTextLength; k >= location && k >= 0; k--) {
    m_textBuffer[k+textSize] = m_textBuffer[k];
  }
  strlcpy(&m_textBuffer[location], text, textSize);
  m_textBuffer[location+textSize-1] = text[textSize-1];
  m_currentTextLength += textSize;
  reload();
}

KDSize TextField::minimalSizeForOptimalDisplay() {
  KDSize textSize = KDText::stringSize(m_textBuffer);
  return KDSize(0, textSize.height());
}

void TextField::setTextFieldDelegate(TextFieldDelegate * delegate) {
  m_delegate = delegate;
}
