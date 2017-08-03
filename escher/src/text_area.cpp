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

/* TextArea::ContentView */

TextArea::ContentView::ContentView(char * textBuffer, size_t textBufferSize, KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
  View(),
  m_text(textBuffer, textBufferSize),
  m_fontSize(fontSize),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor)
{
}

KDSize TextArea::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize charSize = KDText::stringSize(" ", m_fontSize);
  Text::Position span = m_text.span();
  return KDSize(
    charSize.width() * span.column(),
    charSize.height() * span.line()
  );
}

size_t min(size_t a, size_t b) {
  return (a>b ? b : a);
}

void TextArea::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);

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
    if (y >= topLeft.line() && y <= bottomRight.line() && topLeft.column() < line.length()) {
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

/* TextArea */

TextArea::TextArea(Responder * parentResponder, char * textBuffer,
    size_t textBufferSize, TextAreaDelegate * delegate,
    KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
  ScrollableView(parentResponder, &m_contentView, this),
  m_contentView(textBuffer, textBufferSize, fontSize, textColor, backgroundColor),
  m_delegate(delegate)
{
}

View * TextArea::view() {
  return &m_contentView;
}
