#include <escher/text_field.h>
#include <assert.h>

TextField::TextField(Responder * parentResponder, char * textBuffer, char * draftTextBuffer,
    size_t textBufferSize, KDText::FontSize size, TextFieldDelegate * delegate,
    float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  View(),
  Responder(parentResponder),
  m_isEditing(false),
  m_textBuffer(textBuffer),
  m_draftTextBuffer(draftTextBuffer),
  m_currentTextLength(0),
  m_currentCursorLocation(0),
  m_textBufferSize(textBufferSize),
  m_delegate(delegate),
  m_horizontalAlignment(horizontalAlignment),
  m_verticalAlignment(verticalAlignment),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor),
  m_fontSize(size)
{
}

const char * TextField::text() const {
  if (m_isEditing) {
    return (const char *)m_draftTextBuffer;
  }
  return (const char *)m_textBuffer;
}

void TextField::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);
  KDSize textSize = KDText::stringSize(text(), m_fontSize);
  KDPoint origin(m_horizontalAlignment*(m_frame.width() - textSize.width()),
      m_verticalAlignment*(m_frame.height() - textSize.height()));
  ctx->drawString(text(), m_fontSize, origin, m_textColor, m_backgroundColor);
}

#if ESCHER_VIEW_LOGGING
const char * TextField::className() const {
  return "TextField";
}
#endif

void TextField::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void TextField::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void TextField::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  markRectAsDirty(bounds());
}

void TextField::reload() {
  KDSize textSize = KDText::stringSize(text(), m_fontSize);
  KDPoint origin(m_horizontalAlignment*(m_frame.width() - textSize.width()),
    m_verticalAlignment*(m_frame.height() - textSize.height()));
  KDRect dirtyZone(origin, textSize);
  markRectAsDirty(dirtyZone);
}

bool TextField::handleEvent(Ion::Events::Event event) {
  if (m_delegate) {
    if (m_delegate->textFieldDidReceiveEvent(this, event)) {
      return true;
    }
  }
  if (event == Ion::Events::OK) {
    if (m_isEditing) {
      strlcpy(m_textBuffer, m_draftTextBuffer, m_textBufferSize);
      setEditing(false);
      m_delegate->textFieldDidFinishEditing(this, text());
      return true;
    }
    setEditing(true);
    insertTextAtLocation(m_textBuffer, cursorLocation());
    setCursorLocation(strlen(m_draftTextBuffer));
    return true;
  }
  if (event == Ion::Events::Left && m_isEditing) {
    if (m_currentCursorLocation > 0) {
      m_currentCursorLocation--;
    }
    return true;
  }
  if (event == Ion::Events::Right && m_isEditing) {
    if (m_currentCursorLocation < m_currentTextLength) {
      m_currentCursorLocation++;
    }
    return true;
  }
  if (event == Ion::Events::Backspace && m_isEditing) {
    if (m_currentCursorLocation > 0) {
      reload();
      m_currentTextLength--;
      m_currentCursorLocation--;
      for (int k = m_currentCursorLocation; k < m_currentTextLength; k ++) {
      m_draftTextBuffer[k] = m_draftTextBuffer[k+1];
      }
      m_draftTextBuffer[m_currentTextLength] = 0;
    }
    return true;
  }
  if (event.hasText()) {
    if (m_isEditing) {
      insertTextAtLocation(event.text(), cursorLocation());
    } else {
      setEditing(true);
      insertTextAtLocation(event.text(), cursorLocation());
    }
    int cursorDelta = strlen(event.text()) > 1 ? -1 : 0;
    setCursorLocation(cursorLocation() + strlen(event.text()) + cursorDelta);
    return true;
  }
  if (event == Ion::Events::Back && m_isEditing) {
    m_delegate->textFieldDidAbortEditing(this, text());
    setEditing(false);
    return true;
  }
  return false;
}

int TextField::textLength() const {
  assert(strlen(text()) == m_currentTextLength);
  return m_currentTextLength;
}

void TextField::setText(const char * text) {
  reload();
  if (m_isEditing) {
    strlcpy(m_draftTextBuffer, text, m_textBufferSize);
    m_currentCursorLocation = strlen(text);
    m_currentTextLength = m_currentCursorLocation;
  } else {
    strlcpy(m_textBuffer, text, m_textBufferSize);
  }
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
    m_draftTextBuffer[k+textSize] = m_draftTextBuffer[k];
  }
  strlcpy(&m_draftTextBuffer[location], text, textSize);
  m_draftTextBuffer[location+textSize-1] = text[textSize-1];
  m_currentTextLength += textSize;
  reload();
}

KDSize TextField::minimalSizeForOptimalDisplay() {
  KDSize textSize = KDText::stringSize(m_draftTextBuffer, m_fontSize);
  return KDSize(0, textSize.height());
}

void TextField::setTextFieldDelegate(TextFieldDelegate * delegate) {
  m_delegate = delegate;
}

bool TextField::isEditing() const {
  return m_isEditing;
}

void TextField::setEditing(bool isEditing) {
  if (m_isEditing == isEditing) {
    return;
  }
  if (m_isEditing == false) {
    reinitDraftTextBuffer();
  }
  m_isEditing = isEditing;
  markRectAsDirty(bounds());
}

void TextField::reinitDraftTextBuffer() {
  setCursorLocation(0);
  m_draftTextBuffer[0] = 0;
  m_currentTextLength = 0;
}
