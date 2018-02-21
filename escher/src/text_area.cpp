#include <escher/text_area.h>
#include <escher/clipboard.h>
#include <escher/text_input_helpers.h>

#include <stddef.h>
#include <assert.h>
#include <limits.h>


static inline size_t min(size_t a, size_t b) {
  return (a>b ? b : a);
}

TextArea::Text::Text(char * buffer, size_t bufferSize) :
  m_buffer(buffer),
  m_bufferSize(bufferSize)
{
}

void TextArea::Text::setText(char * buffer, size_t bufferSize) {
  m_buffer = buffer;
  m_bufferSize = bufferSize;
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

int TextArea::Text::removeRemainingLine(size_t index, int direction) {
  assert(m_buffer != nullptr);
  assert(index < m_bufferSize);
  int jump = index;
  while (m_buffer[jump] != '\n' && m_buffer[jump] != 0 && jump >= 0) {
    jump += direction;
  }
  int delta = direction > 0 ? jump - index : index - jump;
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

TextArea::ContentView::ContentView(char * textBuffer, size_t textBufferSize, KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
  TextInput::ContentView(fontSize, textColor, backgroundColor),
  m_text(textBuffer, textBufferSize)
{
}

KDSize TextArea::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize charSize = KDText::charSize(m_fontSize);
  Text::Position span = m_text.span();
  return KDSize(
    /* We take into account the space required to draw a cursor at the end of
     * line by adding charSize.width() to the width. */
    charSize.width() * (span.column()+1),
    charSize.height() * span.line()
  );
}


void TextArea::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);

  KDSize charSize = KDText::charSize(m_fontSize);

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

  int y = 0;
  size_t x = topLeft.column();

  for (Text::Line line : m_text) {
    if (y >= topLeft.line() && y <= bottomRight.line() && topLeft.column() < (int)line.length()) {
      //drawString(line.text(), 0, y*charHeight); // Naive version
      ctx->drawString(
        line.text() + topLeft.column(),
        KDPoint(x*charSize.width(), y*charSize.height()),
        m_fontSize,
        m_textColor,
        m_backgroundColor,
        min(line.length() - topLeft.column(), bottomRight.column() - topLeft.column())
      );
    }
    y++;
  }
}

void TextArea::TextArea::ContentView::setText(char * textBuffer, size_t textBufferSize) {
  m_text.setText(textBuffer, textBufferSize);
  m_cursorIndex = 0;
}

const char * TextArea::TextArea::ContentView::text() const {
  return m_text.text();
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
  int removedLine = m_text.removeRemainingLine(cursorLocation(), 1);
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
  int removedLine = m_text.removeRemainingLine(cursorLocation()-1, -1);
  if (removedLine > 0) {
    assert(m_cursorIndex >= removedLine);
    setCursorLocation(cursorLocation()-removedLine);
    reloadRectFromCursorPosition(cursorLocation(), false);
    return true;
  }
  return false;
}

KDRect TextArea::TextArea::ContentView::characterFrameAtIndex(size_t index) const {
  KDSize charSize = KDText::charSize(m_fontSize);
  Text::Position p = m_text.positionAtIndex(index);
  return KDRect(
    p.column() * charSize.width(),
    p.line() * charSize.height(),
    charSize.width(),
    charSize.height()
  );
}

void TextArea::TextArea::ContentView::moveCursorGeo(int deltaX, int deltaY) {
  Text::Position p = m_text.positionAtIndex(m_cursorIndex);
  setCursorLocation(m_text.indexAtPosition(Text::Position(p.column() + deltaX, p.line() + deltaY)));
}

/* TextArea */

TextArea::TextArea(Responder * parentResponder, char * textBuffer,
    size_t textBufferSize, TextAreaDelegate * delegate,
    KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
  TextInput(parentResponder, &m_contentView),
  m_contentView(textBuffer, textBufferSize, fontSize, textColor, backgroundColor),
  m_delegate(delegate)
{
  assert(textBufferSize < INT_MAX/2);
}

bool TextArea::handleEventWithText(const char * text, bool indentation) {
  int nextCursorLocation = cursorLocation();
  if ((indentation && insertTextWithIndentation(text, cursorLocation())) || insertTextAtLocation(text, cursorLocation())) {
    nextCursorLocation += TextInputHelpers::CursorIndexInCommand(text);
  }
  setCursorLocation(nextCursorLocation);
  return true;
}

bool TextArea::handleEvent(Ion::Events::Event event) {
  if (m_delegate != nullptr && m_delegate->textAreaDidReceiveEvent(this, event)) {
    return true;
  } else if (Responder::handleEvent(event)) {
    // The only event Responder handles is 'Toolbox' displaying.
    return true;
  } else if (event == Ion::Events::Left) {
    return setCursorLocation(cursorLocation()-1);
  } else if (event == Ion::Events::Right) {
    return setCursorLocation(cursorLocation()+1);
  } else if (event == Ion::Events::Up) {
    m_contentView.moveCursorGeo(0, -1);
  } else if (event == Ion::Events::Down) {
    m_contentView.moveCursorGeo(0, 1);
  } else if (event == Ion::Events::ShiftLeft) {
    m_contentView.moveCursorGeo(-INT_MAX/2, 0);
  } else if (event == Ion::Events::ShiftRight) {
    m_contentView.moveCursorGeo(INT_MAX/2, 0);
  } else if (event == Ion::Events::Backspace) {
    return removeChar();
  } else if (event.hasText()) {
    return handleEventWithText(event.text());
  } else if (event == Ion::Events::EXE) {
    return handleEventWithText("\n");
  } else if (event == Ion::Events::Clear) {
    if (!m_contentView.removeEndOfLine()) {
      m_contentView.removeStartOfLine();
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
  m_contentView.setText(textBuffer, textBufferSize);
  m_contentView.moveCursorGeo(0, 0);
}

bool TextArea::insertTextWithIndentation(const char * textBuffer, int location) {
  int indentation = indentationBeforeCursor();
  char spaceString[indentation+1]; // WOUHOU
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
  if (m_contentView.getText()->textLength() + textSize + totalIndentationSize >= m_contentView.getText()->bufferSize() || textSize == 0) {
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
  while (charIndex >= 0 && m_contentView.text()[charIndex] != '\n') {
    if (m_contentView.text()[charIndex] == ' ') {
      indentationSize++;
    } else {
      indentationSize = 0;
    }
    charIndex--;
  }
  return indentationSize;
}
