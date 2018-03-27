#include <escher/text_field.h>
#include <escher/text_input_helpers.h>
#include <escher/clipboard.h>
#include <assert.h>

/* TextField::ContentView */

TextField::ContentView::ContentView(char * textBuffer, char * draftTextBuffer, size_t textBufferSize, KDText::FontSize size, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  TextInput::ContentView(size, textColor, backgroundColor),
  m_isEditing(false),
  m_textBuffer(textBuffer),
  m_draftTextBuffer(draftTextBuffer),
  m_currentDraftTextLength(0),
  m_textBufferSize(textBufferSize),
  m_horizontalAlignment(horizontalAlignment),
  m_verticalAlignment(verticalAlignment)
{
  assert(m_textBufferSize <= k_maxBufferSize);
}

void TextField::ContentView::setDraftTextBuffer(char * draftTextBuffer) {
  m_draftTextBuffer = draftTextBuffer;
}

void TextField::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor bckCol = m_backgroundColor;
  if (m_isEditing) {
    bckCol = KDColorWhite;
  }
  ctx->fillRect(rect, bckCol);
  ctx->drawString(text(), characterFrameAtIndex(0).origin(), m_fontSize, m_textColor, bckCol);
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
  if (m_isEditing) {
    strlcpy(m_draftTextBuffer, text, m_textBufferSize);
    int textLength = strlen(text) >= m_textBufferSize ? m_textBufferSize-1 : strlen(text);
    m_currentDraftTextLength = textLength;
  } else {
    strlcpy(m_textBuffer, text, m_textBufferSize);
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
  strlcpy(&m_draftTextBuffer[location], text, textSize);
  if (location+textSize > 0) {
    m_draftTextBuffer[location+textSize-1] = text[textSize-1];
  }
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
  KDSize charSize = KDText::charSize(m_fontSize);
  if (m_isEditing) {
    return KDSize(charSize.width()*strlen(text())+m_cursorView.minimalSizeForOptimalDisplay().width(), charSize.height());
  }
  return KDSize(charSize.width()*strlen(text()), charSize.height());
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
  for (int k = cursorLocation(); k < (signed char)m_currentDraftTextLength; k ++) {
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

void TextField::ContentView::layoutSubviews() {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero);
    return;
  }
  TextInput::ContentView::layoutSubviews();
}

KDRect TextField::ContentView::characterFrameAtIndex(size_t index) const {
  KDSize charSize = KDText::charSize(m_fontSize);
  KDSize textSize = KDText::stringSize(text(), m_fontSize);
  KDCoordinate cursorWidth = m_cursorView.minimalSizeForOptimalDisplay().width();
  return KDRect(m_horizontalAlignment*(m_frame.width() - textSize.width()-cursorWidth)+ index * charSize.width(), m_verticalAlignment*(m_frame.height() - charSize.height()), charSize);
}

/* TextField */

TextField::TextField(Responder * parentResponder, char * textBuffer, char * draftTextBuffer,
    size_t textBufferSize, TextFieldDelegate * delegate, bool hasTwoBuffers, KDText::FontSize size,
    float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  TextInput(parentResponder, &m_contentView),
  m_contentView(textBuffer, draftTextBuffer, textBufferSize, size,horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_hasTwoBuffers(hasTwoBuffers),
  m_delegate(delegate)
{
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
  if (isEditing()) {
    setCursorLocation(draftTextLength());
  }
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
  if (Responder::handleEvent(event)) {
    /* The only event Responder handles is 'Toolbox' displaying. In that case,
     * the text field is forced into editing mode. */
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
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
  if (isEditing() && textFieldShouldFinishEditing(event)) {
    char bufferText[ContentView::k_maxBufferSize];
    strlcpy(bufferText, m_contentView.textBuffer(), ContentView::k_maxBufferSize);
    strlcpy(m_contentView.textBuffer(), m_contentView.draftTextBuffer(), m_contentView.bufferSize());
    int cursorLoc = cursorLocation();
    setEditing(false, m_hasTwoBuffers);
    if (m_delegate->textFieldDidFinishEditing(this, text(), event)) {
      /* We allow overscroll to avoid calling layoutSubviews twice because the
       * content might have changed. */
      reloadScroll(true);
      return true;
    }
    /* if the text was refused (textInputDidFinishEditing returned false, we
     * reset the textfield in the same state as before */
    char bufferDraft[ContentView::k_maxBufferSize];
    strlcpy(bufferDraft, m_contentView.textBuffer(), ContentView::k_maxBufferSize);
    setText(bufferText);
    setEditing(true);
    setText(bufferDraft);
    setCursorLocation(cursorLoc);
    return true;
  }
  if (event == Ion::Events::Backspace && isEditing()) {
    return removeChar();
  }
  if (event == Ion::Events::Back && isEditing()) {
    setEditing(false);
    reloadScroll();
    m_delegate->textFieldDidAbortEditing(this, text());
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    if (!removeEndOfLine()) {
      setEditing(true, true);
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

KDSize TextField::minimalSizeForOptimalDisplay() const {
  return m_contentView.minimalSizeForOptimalDisplay();
}

bool TextField::handleEvent(Ion::Events::Event event) {
  assert(m_delegate != nullptr);
  if (m_delegate->textFieldDidReceiveEvent(this, event)) {
    return true;
  }
  if (event.hasText()) {
    return handleEventWithText(event.text());
  }
  if (event == Ion::Events::Paste) {
    return handleEventWithText(Clipboard::sharedClipboard()->storedText());
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    return handleEventWithText(m_contentView.textBuffer());
  }
  size_t previousTextLength = strlen(text());
  bool didHandleEvent = privateHandleEvent(event);
  return m_delegate->textFieldDidHandleEvent(this, didHandleEvent, strlen(text()) != previousTextLength);
}

void TextField::scrollToCursor() {
  if (!isEditing()) {
    return;
  }
  return TextInput::scrollToCursor();
}

bool TextField::handleEventWithText(const char * eventText, bool indentation) {
  size_t previousTextLength = strlen(text());
  if (!isEditing()) {
    setEditing(true);
  }
  int nextCursorLocation = draftTextLength();
  if (insertTextAtLocation(eventText, cursorLocation())) {
    /* The cursor position depends on the text as we sometimes want to
     * position the cursor at the end of the text and sometimes after the
     * first parenthesis. */
    nextCursorLocation = cursorLocation() + TextInputHelpers::CursorIndexInCommand(eventText);
  }
  setCursorLocation(nextCursorLocation);
  return m_delegate->textFieldDidHandleEvent(this, true, strlen(text()) != previousTextLength);
}
