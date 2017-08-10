#include <escher/text_area.h>
#include <escher/clipboard.h>
#include <assert.h>

#include <stddef.h>
#include <assert.h>


static inline size_t min(size_t a, size_t b) {
  return (a>b ? b : a);
}

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
  if (p.line() < 0) {
    return 0;
  }
  int y = 0;
  const char * endOfLastLine = nullptr;
  for (Line l : *this) {
    if (p.line() == y) {
      size_t x = min(p.column(), l.length());
      return l.text() - m_buffer + x;
    }
    endOfLastLine = l.text() + l.length();
    y++;
  }
  assert(endOfLastLine != nullptr && endOfLastLine >= m_buffer);
  return endOfLastLine - m_buffer;
}

TextArea::Text::Position TextArea::Text::positionAtIndex(size_t index) {
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
  assert(index < m_bufferSize);
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

void TextArea::Text::removeChar(size_t index) {
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

/* TextArea::ContentView */

TextArea::ContentView::ContentView(char * textBuffer, size_t textBufferSize, KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
  View(),
  m_cursorIndex(0),
  m_text(textBuffer, textBufferSize),
  m_fontSize(fontSize),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor)
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

int TextArea::ContentView::numberOfSubviews() const {
  return 1;
}

View * TextArea::ContentView::subviewAtIndex(int index) {
  return &m_cursorView;
}

void TextArea::ContentView::layoutSubviews() {
  m_cursorView.setFrame(cursorRect());
}

void TextArea::TextArea::ContentView::insertText(const char * text) {
  while (*text != 0) {
    m_text.insertChar(*text++, m_cursorIndex++);
  }
  layoutSubviews(); // Reposition the cursor
  markRectAsDirty(bounds()); // FIXME: Vastly suboptimal
}

void TextArea::TextArea::ContentView::removeChar() {
  if (m_cursorIndex > 0) {
    m_text.removeChar(--m_cursorIndex);
  }
  layoutSubviews(); // Reposition the cursor
  markRectAsDirty(bounds()); // FIXME: Vastly suboptimal
}

KDRect TextArea::TextArea::ContentView::cursorRect() {
  return characterFrameAtIndex(m_cursorIndex);
}

KDRect TextArea::TextArea::ContentView::characterFrameAtIndex(size_t index) {
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
  m_cursorIndex = m_text.indexAtPosition(Text::Position(p.column() + deltaX, p.line() + deltaY));
  layoutSubviews();
}

void TextArea::TextArea::ContentView::moveCursorIndex(int deltaX) {
  // FIXME: bound checks!
  m_cursorIndex += deltaX;
  layoutSubviews();
}

/* TextArea */

TextArea::TextArea(Responder * parentResponder, char * textBuffer,
    size_t textBufferSize, TextAreaDelegate * delegate,
    KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
  ScrollableView(parentResponder, &m_contentView, this),
  m_contentView(textBuffer, textBufferSize, fontSize, textColor, backgroundColor),
  m_delegate(delegate)
{
}

bool TextArea::TextArea::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    m_contentView.moveCursorIndex(-1);
  } else if (event == Ion::Events::Right) {
    m_contentView.moveCursorIndex(1);
  } else if (event == Ion::Events::Up) {
    m_contentView.moveCursorGeo(0, -1);
  } else if (event == Ion::Events::Down) {
    m_contentView.moveCursorGeo(0, 1);
  } else if (event == Ion::Events::Backspace) {
    m_contentView.removeChar();
  } else if (event.hasText()) {
    m_contentView.insertText(event.text());
  } else {
    return false;
  }
  scrollToContentRect(m_contentView.cursorRect());
  return true;
}

View * TextArea::view() {
  return &m_contentView;
}
