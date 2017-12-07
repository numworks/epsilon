#include <escher/text_field.h>
#include <escher/clipboard.h>
#include <assert.h>

/* TextField::ContentView */

TextField::ContentView::ContentView(char * textBuffer, char * draftTextBuffer, size_t textBufferSize, KDText::FontSize size, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  View(),
  m_isEditing(false),
  m_textBuffer(textBuffer),
  m_draftTextBuffer(draftTextBuffer),
  m_currentDraftTextLength(0),
  m_currentCursorLocation(0),
  m_textBufferSize(textBufferSize),
  m_horizontalAlignment(horizontalAlignment),
  m_verticalAlignment(verticalAlignment),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor),
  m_fontSize(size)
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
  KDSize textSize = KDText::stringSize(text(), m_fontSize);
  KDPoint origin(m_horizontalAlignment*(m_frame.width() - textSize.width()-m_cursorView.minimalSizeForOptimalDisplay().width()),
      m_verticalAlignment*(m_frame.height() - textSize.height()));
  ctx->drawString(text(), origin, m_fontSize, m_textColor, bckCol);
}

void TextField::ContentView::reload() {
  KDSize textSize = KDText::stringSize(text(), m_fontSize);
  KDPoint origin(m_horizontalAlignment*(m_frame.width() - textSize.width()),
    m_verticalAlignment*(m_frame.height() - textSize.height()));
  KDSize textAndCursorSize = KDSize(textSize.width()+ m_cursorView.minimalSizeForOptimalDisplay().width(), textSize.height());
  KDRect dirtyZone(origin, textAndCursorSize);
  markRectAsDirty(dirtyZone);
}

const char * TextField::ContentView::text() const {
  if (m_isEditing) {
    return const_cast<const char *>(m_draftTextBuffer);
  }
  return const_cast<const char *>(m_textBuffer);
}

int TextField::ContentView::draftTextLength() const {
  assert(isEditing());
  assert(strlen(text()) == m_currentDraftTextLength);
  return m_currentDraftTextLength;
}

void TextField::ContentView::setText(const char * text) {
  if (m_isEditing) {
    strlcpy(m_draftTextBuffer, text, m_textBufferSize);
    int textLength = strlen(text) >= m_textBufferSize ? m_textBufferSize-1 : strlen(text);
    m_currentDraftTextLength = textLength;
  } else {
    strlcpy(m_textBuffer, text, m_textBufferSize);
  }
  reload();
}

void TextField::ContentView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void TextField::ContentView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
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

void TextField::ContentView::setCursorLocation(int location) {
  int adjustedLocation = location < 0 ? 0 : location;
  adjustedLocation = adjustedLocation > (signed int)m_currentDraftTextLength ? (signed int)m_currentDraftTextLength : adjustedLocation;
  m_currentCursorLocation = adjustedLocation;
  layoutSubviews();
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
  reload();
  return true;
}

KDSize TextField::ContentView::minimalSizeForOptimalDisplay() const {
  if (m_isEditing) {
    KDSize textSize = KDText::stringSize(m_draftTextBuffer, m_fontSize);
    return KDSize(textSize.width()+m_cursorView.minimalSizeForOptimalDisplay().width(), textSize.height());
  }
  return KDSize(0, textHeight());
}

KDCoordinate TextField::ContentView::textHeight() const {
  KDSize textSize = KDText::charSize(m_fontSize);
  return textSize.height();
}

KDCoordinate TextField::ContentView::charWidth() {
  KDSize textSize = KDText::charSize(m_fontSize);
  return textSize.width();
}


void TextField::ContentView::deleteCharPrecedingCursor() {
  if (cursorLocation() <= 0) {
    return;
  }
  reload();
  m_currentDraftTextLength--;
  setCursorLocation(m_currentCursorLocation-1);
  for (int k = m_currentCursorLocation; k < (signed char)m_currentDraftTextLength; k ++) {
    m_draftTextBuffer[k] = m_draftTextBuffer[k+1];
  }
  m_draftTextBuffer[m_currentDraftTextLength] = 0;
  layoutSubviews();
}

bool TextField::ContentView::deleteEndOfLine() {
  if (m_currentDraftTextLength == m_currentCursorLocation) {
    return false;
  }
  reload();
  m_currentDraftTextLength = m_currentCursorLocation;
  m_draftTextBuffer[m_currentCursorLocation] = 0;
  layoutSubviews();
  return true;
}

KDRect TextField::ContentView::cursorRect() {
  return KDRect(m_currentCursorLocation * charWidth(), 0, m_cursorView.minimalSizeForOptimalDisplay());
}

void TextField::ContentView::layoutSubviews() {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero);
    return;
  }
  KDSize textSize = KDText::stringSize(text(), m_fontSize);
  KDCoordinate cursorWidth = m_cursorView.minimalSizeForOptimalDisplay().width();
  KDRect frame(m_horizontalAlignment*(m_frame.width() - textSize.width()-cursorWidth)+ m_currentCursorLocation * charWidth(), m_verticalAlignment*(m_frame.height() - textSize.height()), cursorWidth, textSize.height());
  m_cursorView.setFrame(frame);
}

TextField::TextField(Responder * parentResponder, char * textBuffer, char * draftTextBuffer,
    size_t textBufferSize, TextFieldDelegate * delegate, bool hasTwoBuffers, KDText::FontSize size,
    float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  ScrollableView(parentResponder, &m_contentView, this),
  m_contentView(textBuffer, draftTextBuffer, textBufferSize, size,horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_hasTwoBuffers(hasTwoBuffers),
  m_delegate(delegate)
{
}

void TextField::setDelegate(TextFieldDelegate * delegate) {
  m_delegate = delegate;
}

void TextField::setDraftTextBuffer(char * draftTextBuffer) {
  m_contentView.setDraftTextBuffer(draftTextBuffer);
}

Toolbox * TextField::toolbox() {
  if (m_delegate) {
    return m_delegate->toolboxForTextField(this);
  }
  return nullptr;
}

bool TextField::isEditing() const {
  return m_contentView.isEditing();
}

const char * TextField::text() const {
  return m_contentView.text();
}

int TextField::draftTextLength() const {
  assert(isEditing());
  return m_contentView.draftTextLength();
}

int TextField::cursorLocation() const{
  return m_contentView.cursorLocation();
}

void TextField::setCursorLocation(int location) {
  m_contentView.setCursorLocation(location);
  scrollToCursor();
  layoutSubviews();
}

void TextField::setText(const char * text) {
  reloadScroll();
  m_contentView.setText(text);
  if (isEditing()) {
    setCursorLocation(draftTextLength());
  }
}

void TextField::setBackgroundColor(KDColor backgroundColor) {
  m_contentView.setBackgroundColor(backgroundColor);
}

KDColor TextField::backgroundColor() const {
  return m_contentView.backgroundColor();
}

void TextField::setTextColor(KDColor textColor) {
  m_contentView.setTextColor(textColor);
}

void TextField::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_contentView.setAlignment(horizontalAlignment, verticalAlignment);
}

void TextField::setEditing(bool isEditing, bool reinitDrafBuffer) {
  m_contentView.setEditing(isEditing, reinitDrafBuffer);
  if (reinitDrafBuffer) {
    reloadScroll();
    layoutSubviews();
  }
}

bool TextField::insertTextAtLocation(const char * text, int location) {
  if (m_contentView.insertTextAtLocation(text, location)) {
    layoutSubviews();
    return true;
  }
  return false;
}

bool TextField::privateHandleEvent(Ion::Events::Event event) {
    assert(m_delegate != nullptr);
  if (m_delegate->textFieldDidReceiveEvent(this, event)) {
    return true;
  }
  if (Responder::handleEvent(event)) {
    /* The only event Responder handles is 'Toolbox' displaying. In that case,
     * the text field is forced into editing mode. */
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (event == Ion::Events::Left && isEditing() && cursorLocation() > 0) {
    setCursorLocation(cursorLocation()-1);
    return true;
  }
  if (event == Ion::Events::ShiftLeft && isEditing()) {
    setCursorLocation(0);
    return true;
  }
  if (event == Ion::Events::Right && isEditing() && cursorLocation() < draftTextLength()) {
    setCursorLocation(cursorLocation()+1);
    return true;
  }
  if (event == Ion::Events::ShiftRight && isEditing()) {
    setCursorLocation(draftTextLength());
    return true;
  }
  if (isEditing() && textFieldShouldFinishEditing(event)) {
    char bufferText[ContentView::k_maxBufferSize];
    strlcpy(bufferText, m_contentView.textBuffer(), ContentView::k_maxBufferSize);
    strlcpy(m_contentView.textBuffer(), m_contentView.draftTextBuffer(), m_contentView.bufferSize());
    int cursorLoc = cursorLocation();
    setEditing(false, m_hasTwoBuffers);
    if (m_delegate->textFieldDidFinishEditing(this, text(), event)) {
      reloadScroll();
      return true;
    }
    /* if the text was refused (textFieldDidFinishEditing returned false, we
     * reset the textfield in the same state as before */
    char bufferDraft[ContentView::k_maxBufferSize];
    strlcpy(bufferDraft, m_contentView.textBuffer(), ContentView::k_maxBufferSize);
    setText(bufferText);
    setEditing(true);
    setText(bufferDraft);
    setCursorLocation(cursorLoc);
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    setEditing(true);
    /* If the text could not be inserted (buffer is full), we set the cursor
     * at the end of the text. */
    int nextCursorLocation = draftTextLength();
    if (insertTextAtLocation(m_contentView.textBuffer(), cursorLocation())) {
      nextCursorLocation = strlen(m_contentView.draftTextBuffer());
    }
    setCursorLocation(nextCursorLocation);
    return true;
  }
  if (event == Ion::Events::Backspace && isEditing()) {
    deleteCharPrecedingCursor();
    return true;
  }
  if (event.hasText()) {
    if (!isEditing()) {
      setEditing(true);
    }
    int nextCursorLocation = draftTextLength();
    if (insertTextAtLocation(event.text(), cursorLocation())) {
      /* All events whose text is longer than 2 have parenthesis. In these cases,
       * we want to position the cursor before the last parenthesis */
      int cursorDelta = strlen(event.text()) > 2 ? -1 : 0;
      nextCursorLocation = cursorLocation() + strlen(event.text()) + cursorDelta;
    }
    setCursorLocation(nextCursorLocation);
    return true;
  }
  if (event == Ion::Events::Back && isEditing()) {
    setEditing(false);
    reloadScroll();
    m_delegate->textFieldDidAbortEditing(this, text());
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    if (!deleteEndOfLine()) {
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
  if (event == Ion::Events::Paste) {
    if (!isEditing()) {
      setEditing(true);
    }
    int nextCursorLocation = draftTextLength();
    if (insertTextAtLocation(Clipboard::sharedClipboard()->storedText(), cursorLocation())) {
      nextCursorLocation = cursorLocation() + strlen(Clipboard::sharedClipboard()->storedText());
    }
    setCursorLocation(nextCursorLocation);
    return true;
  }

  return false;
}

void TextField::deleteCharPrecedingCursor() {
  m_contentView.deleteCharPrecedingCursor();
  scrollToCursor();
  layoutSubviews();
}

bool TextField::deleteEndOfLine() {
  if (m_contentView.deleteEndOfLine()) {
    scrollToCursor();
    layoutSubviews();
    return true;
  }
  return false;
}

KDSize TextField::minimalSizeForOptimalDisplay() const {
  return KDSize(0, m_contentView.textHeight());
}

bool TextField::textFieldShouldFinishEditing(Ion::Events::Event event) {
  return m_delegate->textFieldShouldFinishEditing(this, event);
}

bool TextField::handleEvent(Ion::Events::Event event) {
  assert(m_delegate != nullptr);
  bool didHandleEvent = privateHandleEvent(event);
  return m_delegate->textFieldDidHandleEvent(this, event, didHandleEvent);
}

void TextField::scrollToCursor() {
  if (!isEditing()) {
    return;
  }
  scrollToContentRect(m_contentView.cursorRect());
}
