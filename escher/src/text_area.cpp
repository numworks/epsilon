#include <escher/text_area.h>
#include <escher/clipboard.h>
#include <assert.h>

#include <stddef.h>
#include <assert.h>

TextArea::Text::Text(char * buffer, size_t bufferSize) :
  m_buffer(buffer),
  m_bufferSize(bufferSize)
{
  assert(m_buffer != nullptr);
}

TextArea::Text::Line::Line(const char * text) :
  m_text(text),
  m_length(0)
{
  if (m_text != nullptr) {
    while (*text != 0 && *text != '\n') {
      text++;
    }
    m_length = text-m_text;
  }
}

bool TextArea::Text::Line::contains(const char * c) const {
  return (c >= m_text) && (c < m_text + m_length);
}

TextArea::Text::LineIterator & TextArea::Text::LineIterator::operator++() {
  const char * last = m_line.text() + m_line.length();
  m_line = Line(*last == 0 ? nullptr : last+1);
  return *this;
}

size_t TextArea::Text::indexAtPosition(Position p) {
  size_t y = 0;
  for (Line l : *this) {
    if (p.line() == y) {
      size_t x = (p.column() > l.length() ? l.length() : p.column());
      return l.text() - m_buffer + x;
    }
    y++;
  }
  assert(false);
  return 0;
}

void TextArea::Text::insert(char c, size_t index) {
  assert(index < m_bufferSize);
  char * cursor = m_buffer + index;
  char previous = c;
  for (size_t i=index; i<m_bufferSize; i++) {
    char inserted = previous;
    previous = m_buffer[i];
    m_buffer[i] = inserted;
    if (inserted == 0) {
      break;
    }
  }
}

void TextArea::Text::remove(size_t index) {
  assert(index < m_bufferSize);
  for (size_t i=index; i<m_bufferSize; i++) {
    m_buffer[i] = m_buffer[i+1];
    if (m_buffer[i] == 0) {
      break;
    }
  }
}

TextArea::Text::Position TextArea::Text::span() const {
  size_t width = 0;
  size_t height = 0;
  for (Line l : *this) {
    if (l.length() > width) {
      width = l.length();
    }
    height++;
  }
  return Position(width, height);
}

#if 0
void drawRect() {
  // drawRect
  //
  Text::Position topLeft;// Compute by division;
  Text::Position bottomRight; // Compute by division too;

  size_t y = 0;
  size_t x = topLeft.col();
  for (Text::Line line : t) {
    if (y >= topLeft.line() && y <= bottomRight.line()) {
      //drawString(line.text(), 0, y*charHeight); // Naive version
      // Only draw what's needed
      drawString(line.text() + topLeft.col(),
          KDPoin(x*charWidth, y*charHeight)
          maxLength = (bottomRight.col() - topLeft.col()));
    }
    y++;
  }
}
#endif

/* TextArea::ContentView */

TextArea::ContentView::ContentView(char * textBuffer, size_t textBufferSize, KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
  View(),
  m_text(textBuffer, textBufferSize),
  m_fontSize(fontSize),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor)
{
}

void TextArea::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  KDSize charSize = KDText::stringSize(" ", m_fontSize);


  // We want to draw even partially visible characters. So we need to round
  // down for the top left corner and up for the bottom right one.
  Text::Position topLeft(
    rect.x()/charSize.width(),
    rect.y()/charSize.height()
  );
  Text::Position bottomRight(
    rect.right()/charSize.width() + 1,
    rect.bottom()/charSize.height() + 1
  );

  size_t y = 0;
  size_t x = topLeft.column();

  for (Text::Line line : m_text) {
    if (y >= topLeft.line() && y <= bottomRight.line()) {
      //drawString(line.text(), 0, y*charHeight); // Naive version
      ctx->drawString(
        line.text() + topLeft.column(),
        KDPoint(x*charSize.width(), y*charSize.height()),
        m_fontSize,
        m_textColor,
        m_backgroundColor
      );
          //maxLength = (bottomRight.col() - topLeft.col()));
    }
    y++;
  }
}

#if 0

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

void TextArea::ContentView::reload() {
  KDSize textSize = KDText::stringSize(text(), m_fontSize);
  KDPoint origin(m_horizontalAlignment*(m_frame.width() - textSize.width()),
    m_verticalAlignment*(m_frame.height() - textSize.height()));
  KDSize textAndCursorSize = KDSize(textSize.width()+ m_cursorView.minimalSizeForOptimalDisplay().width(), textSize.height());
  KDRect dirtyZone(origin, textAndCursorSize);
  markRectAsDirty(dirtyZone);
}

bool TextArea::ContentView::isEditing() const {
  return m_isEditing;
}

const char * TextArea::ContentView::text() const {
  if (m_isEditing) {
    return (const char *)m_draftTextBuffer;
  }
  return (const char *)m_textBuffer;
}

int TextArea::ContentView::textLength() const {
  assert(strlen(text()) == m_currentTextLength);
  return m_currentTextLength;
}

int TextArea::ContentView::cursorLocation() const{
  return m_currentCursorLocation;
}

char * TextArea::ContentView::textBuffer() {
  return m_textBuffer;
}

char * TextArea::ContentView::draftTextBuffer() {
  return m_draftTextBuffer;
}

int TextArea::ContentView::bufferSize() {
  return m_textBufferSize;
}

void TextArea::ContentView::setText(const char * text) {
  if (m_isEditing) {
    strlcpy(m_draftTextBuffer, text, m_textBufferSize);
    int textLength = strlen(text) >= m_textBufferSize ? m_textBufferSize-1 : strlen(text);
    m_currentTextLength = textLength;
  } else {
    strlcpy(m_textBuffer, text, m_textBufferSize);
  }
  reload();
}

void TextArea::ContentView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

KDColor TextArea::ContentView::backgroundColor() const {
  return m_backgroundColor;
}

void TextArea::ContentView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void TextArea::ContentView::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  markRectAsDirty(bounds());
}

void TextArea::ContentView::setEditing(bool isEditing, bool reinitDrafBuffer) {
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

void TextArea::ContentView::reinitDraftTextBuffer() {
  setCursorLocation(0);
  m_draftTextBuffer[0] = 0;
  m_currentTextLength = 0;
}

void TextArea::ContentView::setCursorLocation(int location) {
  int adjustedLocation = location < 0 ? 0 : location;
  adjustedLocation = adjustedLocation > (signed int)m_currentTextLength ? (signed int)m_currentTextLength : adjustedLocation;
  m_currentCursorLocation = adjustedLocation;
  layoutSubviews();
}

bool TextArea::ContentView::insertTextAtLocation(const char * text, int location) {
  int textSize = strlen(text);
  if (m_currentTextLength + textSize >= m_textBufferSize || textSize == 0) {
    return false;
  }
  for (int k = m_currentTextLength; k >= location && k >= 0; k--) {
    m_draftTextBuffer[k+textSize] = m_draftTextBuffer[k];
  }
  strlcpy(&m_draftTextBuffer[location], text, textSize);
  if (location+textSize > 0) {
    m_draftTextBuffer[location+textSize-1] = text[textSize-1];
  }
  m_currentTextLength += textSize;
  reload();
  return true;
}

KDSize TextArea::ContentView::minimalSizeForOptimalDisplay() const {
  if (m_isEditing) {
    KDSize textSize = KDText::stringSize(m_draftTextBuffer, m_fontSize);
    return KDSize(textSize.width()+m_cursorView.minimalSizeForOptimalDisplay().width(), textSize.height());
  }
  return KDSize(0, textHeight());
}

KDCoordinate TextArea::ContentView::textHeight() const {
  KDSize textSize = KDText::stringSize(" ", m_fontSize);
  return textSize.height();
}

KDCoordinate TextArea::ContentView::charWidth() {
  KDSize textSize = KDText::stringSize(" ", m_fontSize);
  return textSize.width();
}


void TextArea::ContentView::deleteCharPrecedingCursor() {
  if (cursorLocation() <= 0) {
    return;
  }
  reload();
  m_currentTextLength--;
  setCursorLocation(m_currentCursorLocation-1);
  for (int k = m_currentCursorLocation; k < (signed char)m_currentTextLength; k ++) {
    m_draftTextBuffer[k] = m_draftTextBuffer[k+1];
  }
  m_draftTextBuffer[m_currentTextLength] = 0;
  layoutSubviews();
}

int TextArea::ContentView::numberOfSubviews() const {
  return 1;
}

View * TextArea::ContentView::subviewAtIndex(int index) {
  return &m_cursorView;
}

void TextArea::ContentView::layoutSubviews() {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero);
    return;
  }
  KDSize textSize = KDText::stringSize(text(), m_fontSize);
  KDCoordinate cursorWidth = m_cursorView.minimalSizeForOptimalDisplay().width();
  KDRect frame(m_horizontalAlignment*(m_frame.width() - textSize.width()-cursorWidth)+ m_currentCursorLocation * charWidth(), m_verticalAlignment*(m_frame.height() - textSize.height()), cursorWidth, textSize.height());
  m_cursorView.setFrame(frame);
}

TextArea::TextArea(Responder * parentResponder, char * textBuffer, char * draftTextBuffer,
    size_t textBufferSize, TextAreaDelegate * delegate, bool hasTwoBuffers, KDText::FontSize size,
    float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  ScrollableView(parentResponder, &m_contentView, this),
  m_contentView(textBuffer, draftTextBuffer, textBufferSize, size,horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_hasTwoBuffers(hasTwoBuffers),
  m_delegate(delegate)
{
}

void TextArea::setDelegate(TextAreaDelegate * delegate) {
  m_delegate = delegate;
}

void TextArea::setDraftTextBuffer(char * draftTextBuffer) {
  m_contentView.setDraftTextBuffer(draftTextBuffer);
}

Toolbox * TextArea::toolbox() {
  if (m_delegate) {
    return m_delegate->toolboxForTextArea(this);
  }
  return nullptr;
}

bool TextArea::isEditing() const {
  return m_contentView.isEditing();
}

const char * TextArea::text() const {
  return m_contentView.text();
}

int TextArea::textLength() const {
  return m_contentView.textLength();
}

int TextArea::cursorLocation() const{
  return m_contentView.cursorLocation();
}

void TextArea::setText(const char * text) {
  reloadScroll();
  m_contentView.setText(text);
  if (isEditing()) {
    setCursorLocation(textLength());
  }
}

void TextArea::setBackgroundColor(KDColor backgroundColor) {
  m_contentView.setBackgroundColor(backgroundColor);
}

KDColor TextArea::backgroundColor() const {
  return m_contentView.backgroundColor();
}

void TextArea::setTextColor(KDColor textColor) {
  m_contentView.setTextColor(textColor);
}

void TextArea::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_contentView.setAlignment(horizontalAlignment, verticalAlignment);
}

void TextArea::setEditing(bool isEditing, bool reinitDrafBuffer) {
  m_contentView.setEditing(isEditing, reinitDrafBuffer);
  if (reinitDrafBuffer) {
    reloadScroll();
    layoutSubviews();
  }
}

void TextArea::setCursorLocation(int location) {
  m_contentView.setCursorLocation(location);
  scrollToCursor();
}

bool TextArea::insertTextAtLocation(const char * text, int location) {
  return m_contentView.insertTextAtLocation(text, location);
}

KDSize TextArea::minimalSizeForOptimalDisplay() const {
  return KDSize(0, m_contentView.textHeight());
}

bool TextArea::textAreaShouldFinishEditing(Ion::Events::Event event) {
  return m_delegate->textAreaShouldFinishEditing(this, event);
}

bool TextArea::handleEvent(Ion::Events::Event event) {
  assert(m_delegate != nullptr);
  if (Responder::handleEvent(event)) {
    /* The only event Responder handles is 'Toolbox' displaying. In that case,
     * the text area is forced into editing mode. */
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (event == Ion::Events::Left && isEditing() && cursorLocation() > 0) {
    setCursorLocation(cursorLocation()-1);
    return true;
  }
  if (event == Ion::Events::Right && isEditing() && cursorLocation() < textLength()) {
    setCursorLocation(cursorLocation()+1);
    return true;
  }
  if (m_delegate->textAreaDidReceiveEvent(this, event)) {
    return true;
  }
  if (textAreaShouldFinishEditing(event) && isEditing()) {
    char bufferText[ContentView::k_maxBufferSize];
    strlcpy(bufferText, m_contentView.textBuffer(), ContentView::k_maxBufferSize);
    strlcpy(m_contentView.textBuffer(), m_contentView.draftTextBuffer(), m_contentView.bufferSize());
    int cursorLoc = cursorLocation();
    setEditing(false, m_hasTwoBuffers);
    if (m_delegate->textAreaDidFinishEditing(this, text(), event)) {
      reloadScroll();
      return true;
    }
    /* if the text was refused (textAreaDidFinishEditing returned false, we
     * reset the textarea in the same state as before */
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
    int nextCursorLocation = textLength();
    if (insertTextAtLocation(m_contentView.textBuffer(), cursorLocation())) {
      nextCursorLocation = strlen(m_contentView.draftTextBuffer());
    }
    setCursorLocation(nextCursorLocation);
    return true;
  }
  if (event == Ion::Events::Backspace && isEditing()) {
    deleteCharPrecedingCursor();
    layoutSubviews();
    return true;
  }
  if (event.hasText()) {
    if (!isEditing()) {
      setEditing(true);
    }
    int nextCursorLocation = textLength();
    if (insertTextAtLocation(event.text(), cursorLocation())) {
      /* All events whose text is longer than 2 have parenthesis. In these cases,
       * we want to position the cursor before the last parenthesis */
      int cursorDelta = strlen(event.text()) > 2 ? -1 : 0;
      nextCursorLocation = cursorLocation() + strlen(event.text()) + cursorDelta;
    }
    setCursorLocation(nextCursorLocation);
    layoutSubviews();
    return true;
  }
  if (event == Ion::Events::Back && isEditing()) {
    setEditing(false);
    reloadScroll();
    m_delegate->textAreaDidAbortEditing(this, text());
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    setEditing(true, true);
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
    int nextCursorLocation = textLength();
    if (insertTextAtLocation(Clipboard::sharedClipboard()->storedText(), cursorLocation())) {
      nextCursorLocation = cursorLocation() + strlen(Clipboard::sharedClipboard()->storedText());
    }
    setCursorLocation(nextCursorLocation);
    return true;
  }


  return false;
}

void TextArea::deleteCharPrecedingCursor() {
  m_contentView.deleteCharPrecedingCursor();
  scrollToAvoidWhiteSpace();
}

bool TextArea::cursorIsBeforeScrollingFrame() {
  return cursorLocation() * m_contentView.charWidth() < m_manualScrolling;
}

bool TextArea::cursorIsAfterScrollingFrame() {
  KDCoordinate cursorWidth = m_contentView.subviewAtIndex(0)->minimalSizeForOptimalDisplay().width();
  return cursorLocation() * m_contentView.charWidth()+cursorWidth - m_manualScrolling > bounds().width();
}

void TextArea::scrollToCursor() {
  if (!isEditing()) {
    return;
  }
  if (cursorIsBeforeScrollingFrame()) {
    m_manualScrolling = cursorLocation() * m_contentView.charWidth();
    setContentOffset(KDPoint(m_manualScrolling, 0));
  }
  if (cursorIsAfterScrollingFrame()) {
    KDCoordinate cursorWidth = m_contentView.subviewAtIndex(0)->minimalSizeForOptimalDisplay().width();
    m_manualScrolling =  cursorLocation() * m_contentView.charWidth()+cursorWidth - bounds().width();
    setContentOffset(KDPoint(m_manualScrolling, 0));
  }
}

void TextArea::scrollToAvoidWhiteSpace() {
  if (m_manualScrolling == 0 || m_manualScrolling + bounds().width() <= textLength() * m_contentView.charWidth()) {
    return;
  }
  KDCoordinate cursorWidth = m_contentView.subviewAtIndex(0)->minimalSizeForOptimalDisplay().width();
  m_manualScrolling = textLength() * m_contentView.charWidth()+cursorWidth-bounds().width();
  if (m_manualScrolling < 0) {
    m_manualScrolling = 0;
  }
  setContentOffset(KDPoint(m_manualScrolling, 0));
}


View * TextArea::view() {
  return &m_contentView;
}#
#endif
