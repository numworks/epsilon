#include <escher/text_area.h>
#include <escher/clipboard.h>
#include <escher/text_input_helpers.h>
#include <kandinsky/unicode/utf8_helper.h>

#include <stddef.h>
#include <assert.h>
#include <limits.h>

static inline size_t min(size_t a, size_t b) {
  return (a>b ? b : a);
}

/* TextArea */

TextArea::TextArea(Responder * parentResponder, View * contentView, const KDFont * font) :
  TextInput(parentResponder, contentView),
  InputEventHandler(nullptr),
  m_delegate(nullptr)
{
}

bool TextArea::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  int nextCursorLocation = cursorLocation();

  size_t cursorIndexInCommand = TextInputHelpers::CursorIndexInCommand(text);

  constexpr int bufferSize = TextField::maxBufferSize();
  char buffer[bufferSize];

  // Remove the Empty code points
  UTF8Helper::CopyAndRemoveCodePoint(buffer, bufferSize, text, KDCodePointEmpty, &cursorIndexInCommand);

  // Insert the text
  if ((indentation && insertTextWithIndentation(buffer, cursorLocation())) || insertTextAtLocation(buffer, cursorLocation())) {
    // Set the cursor location
    if (forceCursorRightOfText) {
      nextCursorLocation += strlen(buffer);
    } else {
      nextCursorLocation += cursorIndexInCommand;
    }
  }
  setCursorLocation(nextCursorLocation);
  return true;
}

bool TextArea::handleEvent(Ion::Events::Event event) {
  if (m_delegate != nullptr && m_delegate->textAreaDidReceiveEvent(this, event)) {
    return true;
  } else if (handleBoxEvent(app(), event)) {
    return true;
  } else if (event == Ion::Events::Left) {
    return setCursorLocation(cursorLocation()-1);
  } else if (event == Ion::Events::Right) {
    return setCursorLocation(cursorLocation()+1);
  } else if (event == Ion::Events::Up) {
    contentView()->moveCursorGeo(0, -1);
  } else if (event == Ion::Events::Down) {
    contentView()->moveCursorGeo(0, 1);
  } else if (event == Ion::Events::ShiftLeft) {
    contentView()->moveCursorGeo(-INT_MAX/2, 0);
  } else if (event == Ion::Events::ShiftRight) {
    contentView()->moveCursorGeo(INT_MAX/2, 0);
  } else if (event == Ion::Events::Backspace) {
    return removeChar();
  } else if (event.hasText()) {
    return handleEventWithText(event.text());
  } else if (event == Ion::Events::EXE) {
    return handleEventWithText("\n");
  } else if (event == Ion::Events::Clear) {
    if (!contentView()->removeEndOfLine()) {
      contentView()->removeStartOfLine();
    }
  } else if (event == Ion::Events::Paste) {
    return handleEventWithText(Clipboard::sharedClipboard()->storedText());
  } else {
    return false;
  }
  scrollToCursor();
  return true;
}

void TextArea::setText(char * textBuffer, size_t textBufferSize) {
  contentView()->setText(textBuffer, textBufferSize);
  contentView()->moveCursorGeo(0, 0);
}

bool TextArea::insertTextWithIndentation(const char * textBuffer, int location) {
  int indentation = indentationBeforeCursor();
  char spaceString[indentation+1];
  for (int i = 0; i < indentation; i++) {
    spaceString[i] = ' ';
  }
  spaceString[indentation] = 0;
  int spaceStringSize = strlen(spaceString);
  int textSize = strlen(textBuffer);
  int totalIndentationSize = 0;
  for (size_t i = 0; i < strlen(textBuffer); i++) {
    if (textBuffer[i] == '\n') {
      totalIndentationSize+=spaceStringSize;
    }
  }
  if (contentView()->getText()->textLength() + textSize + totalIndentationSize >= contentView()->getText()->bufferSize() || textSize == 0) {
    return false;
  }
  int currentLocation = location;
  for (size_t i = 0; i < strlen(textBuffer); i++) {
    const char charString[] = {textBuffer[i], 0};
    insertTextAtLocation(charString, currentLocation++);
    if (textBuffer[i] == '\n') {
      insertTextAtLocation(spaceString, currentLocation);
      currentLocation += strlen(spaceString);
    }
  }
  return true;
}

int TextArea::indentationBeforeCursor() const {
  int charIndex = cursorLocation()-1;
  int indentationSize = 0;
  while (charIndex >= 0 && nonEditableContentView()->text()[charIndex] != '\n') {
    if (nonEditableContentView()->text()[charIndex] == ' ') {
      indentationSize++;
    } else {
      indentationSize = 0;
    }
    charIndex--;
  }
  return indentationSize;
}

/* TextArea::Text */

size_t TextArea::Text::indexAtPosition(Position p) {
  assert(m_buffer != nullptr);
  if (p.line() < 0) {
    return 0;
  }
  int y = 0;
  const char * endOfLastLine = nullptr;
  for (Line l : *this) {
    if (p.line() == y) {
      size_t x = p.column() < 0 ? 0 : p.column();
      x = min(x, l.length());
      return l.text() - m_buffer + x;
    }
    endOfLastLine = l.text() + l.length();
    y++;
  }
  assert(endOfLastLine != nullptr && endOfLastLine >= m_buffer);
  return endOfLastLine - m_buffer;
}

TextArea::Text::Position TextArea::Text::positionAtIndex(size_t index) const {
  assert(m_buffer != nullptr);
  assert(index < m_bufferSize);
  const char * target = m_buffer + index;
  size_t y = 0;
  for (Line l : *this) {
    if (l.text() <= target && l.text() + l.length() >= target) {
      size_t x = target - l.text();
      return Position(x, y);
    }
    y++;
  }
  assert(false);
  return Position(0, 0);
}

void TextArea::Text::insertChar(char c, size_t index) {
  assert(m_buffer != nullptr);
  assert(index < m_bufferSize-1);
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

char TextArea::Text::removeChar(size_t index) {
  assert(m_buffer != nullptr);
  assert(index < m_bufferSize-1);
  char deletedChar = m_buffer[index];
  for (size_t i=index; i<m_bufferSize; i++) {
    m_buffer[i] = m_buffer[i+1];
    if (m_buffer[i] == 0) {
      break;
    }
  }
  return deletedChar;
}

size_t TextArea::Text::removeRemainingLine(size_t index, int direction) {
  assert(m_buffer != nullptr);
  assert(index < m_bufferSize);
  int jump = index;
  while (m_buffer[jump] != '\n' && m_buffer[jump] != 0 && jump >= 0) {
    jump += direction;
  }
  size_t delta = direction > 0 ? jump - index : index - jump;
  if (delta == 0) {
    return 0;
  }
  /* We stop at m_bufferSize-1 because:
   * - if direction > 0: jump >= k+1 so we will reach the 0 before m_bufferSize-1
   * - if direction < 0: k+1 will reach m_bufferSize. */
  for (size_t k = index; k < m_bufferSize-1; k++) {
    if (direction > 0) {
      m_buffer[k] = m_buffer[jump++];
    } else {
      m_buffer[++jump] = m_buffer[k+1];
    }
    if (m_buffer[k] == 0 || m_buffer[k+1] == 0) {
      return delta;
    }
  }
  assert(false);
  return 0;
}

/* TextArea::Text::Line */

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

/* TextArea::Text::LineIterator */

TextArea::Text::LineIterator & TextArea::Text::LineIterator::operator++() {
  const char * last = m_line.text() + m_line.length();
  m_line = Line(*last == 0 ? nullptr : last+1);
  return *this;
}

/* TextArea::Text::Position */

TextArea::Text::Position TextArea::Text::span() const {
  assert(m_buffer != nullptr);
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

/* TextArea::ContentView */

void TextArea::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  // TODO: We're clearing areas we'll draw text over. It's not needed.
  clearRect(ctx, rect);

  KDSize glyphSize = m_font->glyphSize();

  // We want to draw even partially visible characters. So we need to round
  // down for the top left corner and up for the bottom right one.
  Text::Position topLeft(
    rect.x()/glyphSize.width(),
    rect.y()/glyphSize.height()
  );
  Text::Position bottomRight(
    rect.right()/glyphSize.width() + 1,
    rect.bottom()/glyphSize.height() + 1
  );

  int y = 0;

  for (Text::Line line : m_text) {
    if (y >= topLeft.line() && y <= bottomRight.line() && topLeft.column() < (int)line.length()) {
      drawLine(ctx, y, line.text(), line.length(), topLeft.column(), bottomRight.column());
    }
    y++;
  }
}

void TextArea::ContentView::drawStringAt(KDContext * ctx, int line, int column, const char * text, size_t length, KDColor textColor, KDColor backgroundColor) const {
  KDSize glyphSize = m_font->glyphSize();
  ctx->drawString(
    text,
    KDPoint(column*glyphSize.width(), line*glyphSize.height()),
    m_font,
    textColor,
    backgroundColor,
    length
  );
}

KDSize TextArea::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize glyphSize = m_font->glyphSize();
  Text::Position span = m_text.span();
  return KDSize(
    /* We take into account the space required to draw a cursor at the end of
     * line by adding glyphSize.width() to the width. */
    glyphSize.width() * (span.column()+1),
    glyphSize.height() * span.line()
  );
}

void TextArea::TextArea::ContentView::setText(char * textBuffer, size_t textBufferSize) {
  m_text.setText(textBuffer, textBufferSize);
  m_cursorIndex = 0;
}

bool TextArea::TextArea::ContentView::insertTextAtLocation(const char * text, int location) {
  int textSize = strlen(text);
  if (m_text.textLength() + textSize >= m_text.bufferSize() || textSize == 0) {
    return false;
  }
  bool lineBreak = false;
  int currentLocation = location;
  while (*text != 0) {
    lineBreak |= *text == '\n';
    m_text.insertChar(*text++, currentLocation++);
  }
  reloadRectFromCursorPosition(currentLocation-1, lineBreak);
  return true;
}

bool TextArea::TextArea::ContentView::removeChar() {
  if (cursorLocation() <= 0) {
    return false;
  }
  bool lineBreak = false;
  assert(m_cursorIndex > 0);
  lineBreak = m_text.removeChar(--m_cursorIndex) == '\n';
  layoutSubviews(); // Reposition the cursor
  reloadRectFromCursorPosition(cursorLocation(), lineBreak);
  return true;
}

bool TextArea::ContentView::removeEndOfLine() {
  size_t removedLine = m_text.removeRemainingLine(cursorLocation(), 1);
  if (removedLine > 0) {
    layoutSubviews();
    reloadRectFromCursorPosition(cursorLocation(), false);
    return true;
  }
  return false;
}

bool TextArea::ContentView::removeStartOfLine() {
  if (cursorLocation() <= 0) {
    return false;
  }
  size_t removedLine = m_text.removeRemainingLine(cursorLocation()-1, -1);
  if (removedLine > 0) {
    assert(m_cursorIndex >= removedLine);
    setCursorLocation(cursorLocation()-removedLine);
    reloadRectFromCursorPosition(cursorLocation(), false);
    return true;
  }
  return false;
}

KDRect TextArea::ContentView::characterFrameAtIndex(size_t index) const {
  KDSize glyphSize = m_font->glyphSize();
  Text::Position p = m_text.positionAtIndex(index);
  return KDRect(
    p.column() * glyphSize.width(),
    p.line() * glyphSize.height(),
    glyphSize.width(),
    glyphSize.height()
  );
}

void TextArea::ContentView::moveCursorGeo(int deltaX, int deltaY) {
  Text::Position p = m_text.positionAtIndex(m_cursorIndex);
  setCursorLocation(m_text.indexAtPosition(Text::Position(p.column() + deltaX, p.line() + deltaY)));
}
