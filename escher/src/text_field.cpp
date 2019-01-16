#include <escher/text_field.h>
#include <escher/text_input_helpers.h>
#include <escher/clipboard.h>
#include <kandinsky/unicode/utf8_helper.h>
#include <assert.h>

/* TextField::ContentView */

TextField::ContentView::ContentView(char * textBuffer, char * draftTextBuffer, size_t textBufferSize, const KDFont * font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  TextInput::ContentView(font),
  m_isEditing(false),
  m_textBuffer(textBuffer),
  m_draftTextBuffer(draftTextBuffer),
  m_currentDraftTextLength(0),
  m_textBufferSize(textBufferSize),
  m_horizontalAlignment(horizontalAlignment),
  m_verticalAlignment(verticalAlignment),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor)
{
  assert(m_textBufferSize <= k_maxBufferSize);
}

void TextField::ContentView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void TextField::ContentView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void TextField::ContentView::setDraftTextBuffer(char * draftTextBuffer) {
  m_draftTextBuffer = draftTextBuffer;
}

void TextField::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backgroundColor = m_backgroundColor;
  if (m_isEditing) {
    backgroundColor = KDColorWhite;
  }
  ctx->fillRect(bounds(), backgroundColor);
  ctx->drawString(text(), characterFrameAtIndex(0).origin(), m_font, m_textColor, backgroundColor);
}

const char * TextField::ContentView::text() const {
  if (m_isEditing) {
    return const_cast<const char *>(m_draftTextBuffer);
  }
  return const_cast<const char *>(m_textBuffer);
}

size_t TextField::ContentView::editedTextLength() const {
  return m_currentDraftTextLength;
}

void TextField::ContentView::setText(const char * text) {
  reloadRectFromCursorPosition(0);
  size_t textRealLength = strlen(text);
  int textLength = textRealLength >= m_textBufferSize ? m_textBufferSize-1 : textRealLength;
  // Copy the text
  strlcpy(m_isEditing ? m_draftTextBuffer : m_textBuffer, text, m_textBufferSize);
  // Update the draft text length and cursor location
  if (m_isEditing || m_textBuffer == m_draftTextBuffer) {
    m_currentDraftTextLength = textLength;
  }
  reloadRectFromCursorPosition(0);
}

void TextField::ContentView::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  markRectAsDirty(bounds());
}

void TextField::ContentView::setEditing(bool isEditing, bool reinitDrafBuffer) {
  if (m_isEditing == isEditing && !reinitDrafBuffer) {
    return;
  }
  if (reinitDrafBuffer) {
    reinitDraftTextBuffer();
  }
  m_currentDraftTextLength = strlen(m_draftTextBuffer);
  m_isEditing = isEditing;
  markRectAsDirty(bounds());
  layoutSubviews();
}

void TextField::ContentView::reinitDraftTextBuffer() {
  setCursorLocation(0);
  m_draftTextBuffer[0] = 0;
  m_currentDraftTextLength = 0;
}

bool TextField::ContentView::insertTextAtLocation(const char * text, int location) {
  int textSize = strlen(text);
  if (m_currentDraftTextLength + textSize >= m_textBufferSize || textSize == 0) {
    return false;
  }
  for (int k = m_currentDraftTextLength; k >= location && k >= 0; k--) {
    m_draftTextBuffer[k+textSize] = m_draftTextBuffer[k];
  }

  // Caution! One char will be overridden by the null-terminating char of strlcpy
  int overridenCharLocation = location + strlen(text);
  char overridenChar = m_draftTextBuffer[overridenCharLocation];
  strlcpy(&m_draftTextBuffer[location], text, m_textBufferSize-location);
  assert(overridenCharLocation < m_textBufferSize);
  m_draftTextBuffer[overridenCharLocation] = overridenChar;
  m_currentDraftTextLength += textSize;

  for (size_t i = 0; i < m_currentDraftTextLength; i++) {
    if (m_draftTextBuffer[i] == '\n') {
      m_draftTextBuffer[i] = 0;
      m_currentDraftTextLength = i;
      break;
    }
  }
  reloadRectFromCursorPosition((m_horizontalAlignment == 0.0f ? location : 0));
  return true;
}

KDSize TextField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize glyphSize = m_font->glyphSize();
  if (m_isEditing) {
    return KDSize(glyphSize.width()*strlen(text())+m_cursorView.minimalSizeForOptimalDisplay().width(), glyphSize.height());
  }
  return KDSize(glyphSize.width()*strlen(text()), glyphSize.height());
}

bool TextField::ContentView::removeChar() {
  if (cursorLocation() <= 0) {
    return false;
  }
  m_currentDraftTextLength--;
  if (m_horizontalAlignment > 0.0f) {
    reloadRectFromCursorPosition(0);
  }
  setCursorLocation(cursorLocation()-1);
  if( m_horizontalAlignment == 0.0f) {
    reloadRectFromCursorPosition(cursorLocation());
  }
  for (size_t k = cursorLocation(); k < m_currentDraftTextLength; k++) {
    m_draftTextBuffer[k] = m_draftTextBuffer[k+1];
  }
  m_draftTextBuffer[m_currentDraftTextLength] = 0;
  layoutSubviews();
  return true;
}

bool TextField::ContentView::removeEndOfLine() {
  if (m_currentDraftTextLength == cursorLocation()) {
    return false;
  }
  reloadRectFromCursorPosition((m_horizontalAlignment == 0.0f ? cursorLocation() : 0));
  m_currentDraftTextLength = cursorLocation();
  m_draftTextBuffer[cursorLocation()] = 0;
  layoutSubviews();
  return true;
}

void TextField::ContentView::willModifyTextBuffer() {
  /* This method should be called when the buffer is modified outside the
   * content view, for instance from the textfield directly. */
  reloadRectFromCursorPosition(0);
}

void TextField::ContentView::didModifyTextBuffer() {
  /* This method should be called when the buffer is modified outside the
   * content view, for instance from the textfield directly. */
  m_currentDraftTextLength = strlen(m_draftTextBuffer);
  layoutSubviews();
}

void TextField::ContentView::layoutSubviews() {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero);
    return;
  }
  TextInput::ContentView::layoutSubviews();
}

KDRect TextField::ContentView::characterFrameAtIndex(size_t index) const {
  KDSize glyphSize = m_font->glyphSize();
  KDSize textSize = m_font->stringSize(text());
  KDCoordinate cursorWidth = m_cursorView.minimalSizeForOptimalDisplay().width();
  return KDRect(m_horizontalAlignment*(m_frame.width() - textSize.width()-cursorWidth)+ index * glyphSize.width(), m_verticalAlignment*(m_frame.height() - glyphSize.height()), glyphSize);
}

/* TextField */

TextField::TextField(Responder * parentResponder, char * textBuffer, char * draftTextBuffer,
    size_t textBufferSize, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate, bool hasTwoBuffers, const KDFont * font,
    float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  TextInput(parentResponder, &m_contentView),
  EditableField(inputEventHandlerDelegate),
  m_contentView(textBuffer, draftTextBuffer, textBufferSize, font, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_hasTwoBuffers(hasTwoBuffers),
  m_delegate(delegate)
{
}

void TextField::setBackgroundColor(KDColor backgroundColor) {
  ScrollView::setBackgroundColor(backgroundColor);
  m_contentView.setBackgroundColor(backgroundColor);
}

void TextField::setTextColor(KDColor textColor) {
  m_contentView.setTextColor(textColor);
}

void TextField::setDraftTextBuffer(char * draftTextBuffer) {
  m_contentView.setDraftTextBuffer(draftTextBuffer);
}

bool TextField::isEditing() const {
  return m_contentView.isEditing();
}

size_t TextField::draftTextLength() const {
  assert(isEditing());
  return m_contentView.editedTextLength();
}

void TextField::setText(const char * text) {
  reloadScroll();
  m_contentView.setText(text);
  /* Set the cursor location here and not in ContentView::setText so that
   * TextInput::willSetCursorLocation is called. */
  setCursorLocation(strlen(text));
}

void TextField::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_contentView.setAlignment(horizontalAlignment, verticalAlignment);
}

void TextField::setEditing(bool isEditing, bool reinitDrafBuffer) {
  m_contentView.setEditing(isEditing, reinitDrafBuffer);
  if (reinitDrafBuffer) {
    reloadScroll();
  }
}

bool TextField::privateHandleEvent(Ion::Events::Event event) {
  // Handle Toolbox or Var event
  if (handleBoxEvent(app(), event)) {
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && shouldFinishEditing(event)) {
    char bufferText[ContentView::k_maxBufferSize];
    int cursorLoc = cursorLocation();
    if (m_hasTwoBuffers) {
      strlcpy(bufferText, m_contentView.textBuffer(), ContentView::k_maxBufferSize);
      strlcpy(m_contentView.textBuffer(), m_contentView.draftTextBuffer(), m_contentView.bufferSize());
    }
    /* If textFieldDidFinishEditing displays a pop-up (because of an unvalid
     * text for instance), the text field will call willResignFirstResponder.
     * This will call textFieldDidAbortEditing if the textfield is still editing,
     * which we do not want, as we are not really aborting edition, just
     * displaying a pop-up before returning to edition.
     * We thus set editing to false. */
    setEditing(false, m_hasTwoBuffers);
    if (m_delegate->textFieldDidFinishEditing(this, text(), event)) {
      /* We allow overscroll to avoid calling layoutSubviews twice because the
       * content might have changed. */
      reloadScroll(true);
      return true;
    }
    setEditing(true, false);
    if (m_hasTwoBuffers) {
      /* if the text was refused (textInputDidFinishEditing returned false, we
       * reset the textfield in the same state as before */
      setText(m_contentView.textBuffer());
      strlcpy(m_contentView.textBuffer(), bufferText, ContentView::k_maxBufferSize);
      setCursorLocation(cursorLoc);
    }
    return true;
  }
  /* if move event was not caught before nor by textFieldShouldFinishEditing,
   * we handle it here to avoid bubbling the event up. */
  if ((event == Ion::Events::Up || event == Ion::Events::Down || event == Ion::Events::Left || event == Ion::Events::Right) && isEditing()) {
    return true;
  }
  if (event == Ion::Events::Backspace && isEditing()) {
    return removeChar();
  }
  if (event == Ion::Events::Back && isEditing()) {
    setEditing(false, m_hasTwoBuffers);
    m_delegate->textFieldDidAbortEditing(this);
    reloadScroll(true);
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    if (!removeEndOfLine()) {
      removeWholeText();
    }
    return true;
  }
  if (event == Ion::Events::Copy && !isEditing()) {
    Clipboard::sharedClipboard()->store(text());
    return true;
  }
  if (event == Ion::Events::Cut && !isEditing()) {
    Clipboard::sharedClipboard()->store(text());
    setEditing(true, true);
    return true;
  }
  return false;
}

CodePoint TextField::XNTCodePoint(CodePoint defaultXNTCodePoint) {
  static constexpr struct { const char *name; char xnt; } sFunctions[] = {
    { "diff", 'x' }, { "int", 'x' },
    { "product", 'n' }, { "sum", 'n' }
  };
  // Let's assume everything before the cursor is nested correctly, which is reasonable if the expression is being entered left-to-right.
  const char * text = this->text();
  /* TODO LEA
  size_t location = cursorLocation();
  unsigned level = 0;
  while (location >= 1) {
    location--;
    switch (text[location]) {
      case '(':
        // Check if we are skipping to the next matching '('.
        if (level) {
          level--;
          break;
        }
        // Skip over whitespace.
        while (location >= 1 && text[location-1] == ' ') {
          location--;
        }
        // We found the next innermost function we are currently in.
        for (size_t i = 0; i < sizeof(sFunctions)/sizeof(sFunctions[0]); i++) {
          const char * name = sFunctions[i].name;
          size_t length = strlen(name);
          if (location >= length && memcmp(&text[location-length], name, length) == 0) {
            return sFunctions[i].xnt;
          }
        }
        break;
      case ',':
        // Commas encountered while skipping to the next matching '(' should be ignored.
        if (level) {
          break;
        }
        // FALLTHROUGH
      case ')':
        // Skip to the next matching '('.
        level++;
        break;
    }
  }
  */
  // Fallback to the default
  return defaultXNTCodePoint;
}

bool TextField::handleEvent(Ion::Events::Event event) {
  assert(m_delegate != nullptr);
  size_t previousTextLength = strlen(text());
  bool didHandleEvent = false;
  if (privateHandleMoveEvent(event)) {
    didHandleEvent = true;
  } else if (m_delegate->textFieldDidReceiveEvent(this, event)) {
    return true;
  } else if (event.hasText()) {
    return handleEventWithText(event.text());
  } else if (event == Ion::Events::Paste) {
    return handleEventWithText(Clipboard::sharedClipboard()->storedText());
  } else if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    return handleEventWithText(m_contentView.textBuffer());
  }
  if (!didHandleEvent) {
    didHandleEvent = privateHandleEvent(event);
  }
  return m_delegate->textFieldDidHandleEvent(this, didHandleEvent, strlen(text()) != previousTextLength);
}

void TextField::scrollToCursor() {
  if (!isEditing()) {
    return;
  }
  return TextInput::scrollToCursor();
}

bool TextField::privateHandleMoveEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left && isEditing() && cursorLocation() > 0) {
    return setCursorLocation(cursorLocation()-1);
  }
  if (event == Ion::Events::ShiftLeft && isEditing()) {
    return setCursorLocation(0);
  }
  if (event == Ion::Events::Right && isEditing() && cursorLocation() < draftTextLength()) {
    return setCursorLocation(cursorLocation()+1);
  }
  if (event == Ion::Events::ShiftRight && isEditing()) {
    return setCursorLocation(draftTextLength());
  }
  return false;
}

bool TextField::handleEventWithText(const char * eventText, bool indentation, bool forceCursorRightOfText) {
  size_t previousTextLength = strlen(text());

  if (!isEditing()) {
    setEditing(true);
  }

  if (eventText[0] == 0) {
    setCursorLocation(0);
    return m_delegate->textFieldDidHandleEvent(this, true, previousTextLength != 0);
  }

  // Remove the Empty code points
  constexpr int bufferSize = TextField::maxBufferSize();
  char buffer[bufferSize];
  UTF8Helper::CopyAndRemoveCodePoint(buffer, bufferSize, eventText, KDCodePointEmpty);

  int nextCursorLocation = draftTextLength();
  if (insertTextAtLocation(buffer, cursorLocation())) {
    /* The cursor position depends on the text as we sometimes want to position
     * the cursor at the end of the text and sometimes after the first
     * parenthesis. */
    nextCursorLocation = cursorLocation();
    if (forceCursorRightOfText) {
      nextCursorLocation+= strlen(buffer);
    } else {
      nextCursorLocation+= TextInputHelpers::CursorIndexInCommand(eventText);
    }
  }
  setCursorLocation(nextCursorLocation);
  return m_delegate->textFieldDidHandleEvent(this, true, strlen(text()) != previousTextLength);
}

void TextField::removeWholeText() {
  setEditing(true, true);
}
