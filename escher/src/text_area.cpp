#include <escher/text_area.h>
#include <escher/clipboard.h>
#include <escher/text_input_helpers.h>
#include <kandinsky/unicode/utf8_decoder.h>
#include <kandinsky/unicode/utf8_helper.h>

#include <stddef.h>
#include <assert.h>
#include <limits.h>

static inline size_t minSize(size_t a, size_t b) { return a < b ? a : b; }
static inline size_t maxSize(size_t a, size_t b) { return a > b ? a : b; }

/* TextArea */

TextArea::TextArea(Responder * parentResponder, View * contentView, const KDFont * font) :
  TextInput(parentResponder, contentView),
  InputEventHandler(nullptr),
  m_delegate(nullptr)
{
}

bool TextArea::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  const char * nextCursorLocation = cursorTextLocation();

  const char * cursorPositionInCommand = TextInputHelpers::CursorPositionInCommand(text);

  constexpr int bufferSize = TextField::maxBufferSize();
  char buffer[bufferSize];

  // Remove the Empty code points
  UTF8Helper::CopyAndRemoveCodePoint(buffer, bufferSize, text, KDCodePointEmpty, &cursorPositionInCommand);

  // Insert the text
  if ((indentation && insertTextWithIndentation(buffer, cursorTextLocation())) || insertTextAtLocation(buffer, cursorTextLocation())) {
    // Set the cursor location
    if (forceCursorRightOfText) {
      nextCursorLocation += strlen(buffer);
    } else {
      nextCursorLocation += cursorPositionInCommand - text;
    }
  }
  setCursorTextLocation(nextCursorLocation);
  return true;
}

bool TextArea::handleEvent(Ion::Events::Event event) {
  if (m_delegate != nullptr && m_delegate->textAreaDidReceiveEvent(this, event)) {
    return true;
  } else if (handleBoxEvent(app(), event)) {
    return true;
  } else if (event == Ion::Events::Left) {
    if (cursorTextLocation() <= text()) {
      assert(cursorTextLocation() == text());
      return false;
    }
    UTF8Decoder decoder(text(), cursorTextLocation());
    decoder.previousCodePoint();
    return setCursorTextLocation(decoder.stringPosition());
  } else if (event == Ion::Events::Right) {
    if (*cursorTextLocation() == 0) {
      return false;
    }
    UTF8Decoder decoder(text(), cursorTextLocation());
    decoder.nextCodePoint();
    return setCursorTextLocation(decoder.stringPosition());
  } else if (event == Ion::Events::Up) {
    contentView()->moveCursorGeo(0, -1);
  } else if (event == Ion::Events::Down) {
    contentView()->moveCursorGeo(0, 1);
  } else if (event == Ion::Events::ShiftLeft) {
    contentView()->moveCursorGeo(-INT_MAX/2, 0);
  } else if (event == Ion::Events::ShiftRight) {
    contentView()->moveCursorGeo(INT_MAX/2, 0);
  } else if (event == Ion::Events::Backspace) {
    return removeCodePoint();
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

bool TextArea::insertTextWithIndentation(const char * textBuffer, const char * location) {
  int indentation = indentationBeforeCursor();
  const char * previousChar = cursorTextLocation()-1;
  if (previousChar >= const_cast<TextArea *>(this)->contentView()->text() && *previousChar == ':') {
    indentation += k_indentationSpaces;
  }
  int textSize = strlen(textBuffer);
  int totalIndentationSize = 0;
  for (size_t i = 0; i < textSize; i++) {
    // No need to use the UTF8Decoder here, because we look for an ASCII char.
    if (textBuffer[i] == '\n') {
      totalIndentationSize+= indentation;
    }
  }
  if (contentView()->getText()->textLength() + textSize + totalIndentationSize >= contentView()->getText()->bufferSize() || textSize == 0) {
    return false;
  }
  insertTextAtLocation(textBuffer, location);
  const char * currentLocation = location;
  int currentIndex = location - contentView()->text();
  for (size_t i = 0; i < textSize; i++) {
    // No need to use the UTF8Decoder here, because we look for an ASCII char.
    if (textBuffer[currentIndex + i] == '\n') {
      const_cast<Text *>(contentView()->getText())->insertSpacesAtLocation(indentation, const_cast<char *>(currentLocation));
      currentIndex+= indentation;
    }
  }
  return true;
}

int TextArea::indentationBeforeCursor() const {
  const char * p = cursorTextLocation()-1;
  int indentationSize = 0;
  // No need to use the UTF8Decoder here, be cause we look for an ASCII char.
  while (p >= const_cast<TextArea *>(this)->contentView()->text() && *p != '\n') {
    if (*p == ' ') {
      indentationSize++;
    } else {
      indentationSize = 0;
    }
    p--;
  }
  return indentationSize;
}

/* TextArea::Text */

const char * TextArea::Text::pointerAtPosition(Position p) {
  assert(m_buffer != nullptr);
  if (p.line() < 0) {
    return m_buffer;
  }
  int y = 0;
  for (Line l : *this) {
    if (p.line() == y) {
      return l.text() + minSize(l.charLength(), maxSize(p.column(), 0));
    }
    y++;
  }
  return m_buffer + strlen(m_buffer);
}

TextArea::Text::Position TextArea::Text::positionAtPointer(const char * p) const {
  assert(m_buffer != nullptr);
  assert(m_buffer <= p && p < m_buffer + m_bufferSize);
  size_t y = 0;
  for (Line l : *this) {
    if (l.contains(p)) {
      size_t x = p - l.text();
      return Position(x, y);
    }
    y++;
  }
  assert(false);
  return Position(0, 0);
}

void TextArea::Text::insertText(const char * s, int textLength, char * location) {
  assert(m_buffer != nullptr);
  assert(location >= m_buffer && location < m_buffer + m_bufferSize - 1);
  assert(strlen(m_buffer) + textLength < m_bufferSize);

  size_t sizeToMove = strlen(location) + 1;
  assert(location + textLength + sizeToMove <= m_buffer + m_bufferSize);
  memmove(location + textLength, location, sizeToMove);
  memmove(location, s, textLength);
}

void TextArea::Text::insertSpacesAtLocation(int numberOfSpaces, char * location) {
  assert(m_buffer != nullptr);
  assert(location >= m_buffer && location < m_buffer + m_bufferSize - 1);
  assert(strlen(m_buffer) + numberOfSpaces < m_bufferSize);

  size_t sizeToMove = strlen(location) + 1;
  assert(location + numberOfSpaces + sizeToMove <= m_buffer + m_bufferSize);
  memmove(location + numberOfSpaces, location, sizeToMove);
  for (int i = 0; i < numberOfSpaces; i++) {
    *(location+i) = ' ';
  }
}

CodePoint TextArea::Text::removeCodePoint(const char * * position) {
  assert(m_buffer != nullptr);
  assert(m_buffer <= *position && *position < m_buffer + m_bufferSize);

  // Find the beginning of the previous code point
  UTF8Decoder decoder(m_buffer, *position);
  CodePoint deletedCodePoint = decoder.previousCodePoint();
  const char * newCursorLocation = decoder.stringPosition();
  assert(newCursorLocation < *position);

  // Shift the buffer
  int codePointSize = *position - newCursorLocation;
  assert(newCursorLocation >= m_buffer);
  for (size_t i = newCursorLocation - m_buffer; i < m_bufferSize; i++) {
    m_buffer[i] = m_buffer[i + codePointSize];
    if (m_buffer[i] == 0) {
      break;
    }
  }
  // Set the new cursor position
  *position = newCursorLocation;
  return deletedCodePoint;
}

size_t TextArea::Text::removeRemainingLine(const char * location, int direction) {
  assert(m_buffer != nullptr);
  assert(location >= m_buffer && location < m_buffer + m_bufferSize);

  UTF8Decoder decoder(m_buffer, location);
  const char * codePointPosition = decoder.stringPosition();
  CodePoint nextCodePoint = direction > 0 ? decoder.nextCodePoint() : decoder.previousCodePoint();
  if (direction < 0) {
    codePointPosition = decoder.stringPosition();
  }
  while (nextCodePoint != '\n'
      && ((direction > 0 && nextCodePoint != 0)
        || (direction < 0 && codePointPosition >= m_buffer)))
  {
    if (direction > 0) {
      codePointPosition = decoder.stringPosition();
    }
    nextCodePoint = direction > 0 ? decoder.nextCodePoint() : decoder.previousCodePoint();
    if (direction < 0) {
      codePointPosition = decoder.stringPosition();
    }
  }
  size_t delta = direction > 0 ? codePointPosition - location : location - codePointPosition;
  if (delta == 0) {
    return 0;
  }

  char * dst = const_cast<char* >(direction > 0 ? location : codePointPosition);
  char * src = const_cast<char* >(direction > 0 ? codePointPosition : location);
  assert(src > dst);
  for (size_t index = src - m_buffer; index < m_bufferSize; index++) {
    *dst = *src;
    if (*src == 0) {
      return delta;
    }
    dst++;
    src++;
  }
  assert(false);
  return 0;
}

/* TextArea::Text::Line */

TextArea::Text::Line::Line(const char * text) :
  m_text(text),
  m_charLength(0)
{
  if (m_text != nullptr) {
    // No need to use the UTF8Decoder here, because we look for an ASCII char.
    while (*text != 0 && *text != '\n') {
      text++;
    }
    m_charLength = text - m_text;
  }
}

KDCoordinate TextArea::Text::Line::glyphWidth(const KDFont * const font) const {
  return font->stringSizeUntil(m_text, m_text + m_charLength).width();
}

bool TextArea::Text::Line::contains(const char * c) const {
  return (c >= m_text)
    && ((c < m_text + m_charLength)
        || (c == m_text + m_charLength && (*c == 0 || *c == '\n'))) ;
}

/* TextArea::Text::LineIterator */

TextArea::Text::LineIterator & TextArea::Text::LineIterator::operator++() {
  const char * last = m_line.text() + m_line.charLength();
  m_line = Line(*last == 0 ? nullptr : last+1);
  return *this;
}

/* TextArea::Text::Position */

KDSize TextArea::Text::span(const KDFont * const font) const {
  assert(m_buffer != nullptr);
  KDCoordinate width = 0;
  int numberOfLines = 0;
  for (Line l : *this) {
    KDCoordinate lineWidth = l.glyphWidth(font);
    if (lineWidth > width) {
      width = lineWidth;
    }
    numberOfLines++;
  }
  return KDSize(width, numberOfLines * font->glyphSize().height());
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
    KDCoordinate width = line.glyphWidth(m_font);
    if (y >= topLeft.line() && y <= bottomRight.line() && topLeft.column() < (int)width) {
      drawLine(ctx, y, line.text(), width, topLeft.column(), bottomRight.column());
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
  KDSize span = m_text.span(m_font);
  return KDSize(
    /* We take into account the space required to draw a cursor at the end of
     * line by adding glyphSize.width() to the width. */
    span.width() + m_font->glyphSize().width(),
    span.height()
  );
}

void TextArea::TextArea::ContentView::setText(char * textBuffer, size_t textBufferSize) {
  m_text.setText(textBuffer, textBufferSize);
  m_cursorTextLocation = text();
}

bool TextArea::TextArea::ContentView::insertTextAtLocation(const char * text, const char * location) {
  int textSize = strlen(text);
  if (m_text.textLength() + textSize >= m_text.bufferSize() || textSize == 0) {
    return false;
  }
  bool lineBreak = false;

  // Scan for \n and 0
  const char * textScanner = text;
  while (*textScanner != 0) {
    textScanner++;
    lineBreak |= *textScanner == '\n';
  }
  assert(*textScanner == 0);
  m_text.insertText(text, textScanner - text, const_cast<char *>(location));
  reloadRectFromPosition(location/*-1 TODO  LEA */, lineBreak);
  return true;
}

bool TextArea::TextArea::ContentView::removeCodePoint() {
  if (cursorTextLocation() <= text()) {
    assert(cursorTextLocation() == text());
    return false;
  }
  bool lineBreak = false;
  const char * cursorLoc = cursorTextLocation();
  lineBreak = m_text.removeCodePoint(&cursorLoc) == '\n';
  setCursorTextLocation(cursorLoc); // Update the cursor
  layoutSubviews(); // Reposition the cursor
  reloadRectFromPosition(cursorTextLocation(), lineBreak);
  return true;
}

bool TextArea::ContentView::removeEndOfLine() {
  size_t removedLine = m_text.removeRemainingLine(cursorTextLocation(), 1);
  if (removedLine > 0) {
    layoutSubviews();
    reloadRectFromPosition(cursorTextLocation(), false);
    return true;
  }
  return false;
}

bool TextArea::ContentView::removeStartOfLine() {
  if (cursorTextLocation() <= text()) {
    assert(cursorTextLocation() == text());
    return false;
  }
  size_t removedLine = m_text.removeRemainingLine(cursorTextLocation(), -1); //TODO LEA Before : cursorTextLocation()-1
  if (removedLine > 0) {
    assert(cursorTextLocation() >= text() + removedLine);
    setCursorTextLocation(cursorTextLocation() - removedLine);
    reloadRectFromPosition(cursorTextLocation(), false);
    return true;
  }
  return false;
}

KDRect TextArea::ContentView::glyphFrameAtPosition(const char * position) const {
  KDSize glyphSize = m_font->glyphSize();
  Text::Position p = m_text.positionAtPointer(position);

  KDCoordinate x = 0;
  bool found = false;
  int y = 0;
  for (Text::Line l : m_text) {
    if (p.line() == y) {
      x = m_font->stringSizeUntil(l.text(), position).width();
      found = true;
      break;
    }
    y++;
  }
  assert(found);

  return KDRect(
    x,
    p.line() * glyphSize.height(),
    glyphSize.width(),
    glyphSize.height()
  );
}

void TextArea::ContentView::moveCursorGeo(int deltaX, int deltaY) {
  Text::Position p = m_text.positionAtPointer(cursorTextLocation());
  setCursorTextLocation(m_text.pointerAtPosition(Text::Position(p.column() + deltaX, p.line() + deltaY)));
}
