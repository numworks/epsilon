#include <escher/text_area.h>
#include <escher/clipboard.h>
#include <escher/text_input_helpers.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/serialization_helper.h>

#include <stddef.h>
#include <assert.h>
#include <limits.h>
#include <algorithm>

/* TextArea */

TextArea::TextArea(Responder * parentResponder, View * contentView, const KDFont * font) :
  TextInput(parentResponder, contentView),
  InputEventHandler(nullptr),
  m_delegate(nullptr)
{
}

static inline void InsertSpacesAtLocation(int spacesCount, char * buffer, int bufferSize) {
  assert(buffer != nullptr);
  assert((int)(strlen(buffer) + spacesCount) < bufferSize);

  size_t sizeToMove = strlen(buffer) + 1;
  size_t spaceCharSize = UTF8Decoder::CharSizeOfCodePoint(' ');
  size_t spacesLength = spacesCount * spaceCharSize;
  memmove(buffer + spacesLength, buffer, sizeToMove);
  for (int i = 0; i < spacesCount; i++) {
    int spaceOffset = i * spaceCharSize;
    UTF8Decoder::CodePointToChars(' ', buffer + spaceOffset, bufferSize - spaceOffset);
  }
}

bool TextArea::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  if (*text == 0) {
    return false;
  }

  // Delete the selected text if needed
  if (!contentView()->selectionIsEmpty()) {
    deleteSelection();
  }

  /* Compute the indentation. If the text cannot be inserted with the
   * indentation, stop here. */
  int spacesCount = 0;
  int totalIndentationSize = 0;
  int textLen = strlen(text);
  char * insertionPosition = const_cast<char *>(cursorLocation());
  if (indentation) {
    // Compute the indentation
    spacesCount = indentationBeforeCursor();
    const char * textAreaBuffer = contentView()->text();
    if (insertionPosition > textAreaBuffer && UTF8Helper::PreviousCodePointIs(textAreaBuffer, insertionPosition, ':')) {
      spacesCount += k_indentationSpaces;
    }
    // Check the text will not overflow the buffer
    totalIndentationSize = UTF8Helper::CountOccurrences(text, '\n') * spacesCount;
    if (contentView()->getText()->textLength() + textLen + totalIndentationSize >= contentView()->getText()->bufferSize()) {
      return false;
    }
  }

  // Insert the text
  if (!insertTextAtLocation(text, insertionPosition)) {
    return true;
  }

  // Insert the indentation
  if (indentation) {
    UTF8Helper::PerformAtCodePoints(
        insertionPosition,
        '\n',
        [](int codePointOffset, void * text, int indentation, int bufferLength) {
          int offset = codePointOffset + UTF8Decoder::CharSizeOfCodePoint('\n');
          InsertSpacesAtLocation(indentation, (char *)text + offset, bufferLength);
        },
        [](int c1, void * c2, int c3, int c4) {},
        (void *)insertionPosition,
        spacesCount,
        contentView()->getText()->bufferSize() - (insertionPosition - contentView()->getText()->text()),
        UCodePointNull,
        true,
        nullptr,
        insertionPosition + textLen);
  }
  const char * endOfInsertedText = insertionPosition + textLen + totalIndentationSize;
  const char * cursorPositionInCommand = TextInputHelpers::CursorPositionInCommand(insertionPosition, endOfInsertedText);

  // Remove the Empty code points
  UTF8Helper::RemoveCodePoint(insertionPosition, UCodePointEmpty, &cursorPositionInCommand, endOfInsertedText);

  // Set the cursor location
  const char * nextCursorLocation = forceCursorRightOfText ? endOfInsertedText : cursorPositionInCommand;
  setCursorLocation(nextCursorLocation);

  return true;
}

bool TextArea::handleEvent(Ion::Events::Event event) {
  if (m_delegate != nullptr && m_delegate->textAreaDidReceiveEvent(this, event)) {
    return true;
  }
  if (handleBoxEvent(event)) {
    return true;
  }
  if (event == Ion::Events::ShiftLeft || event == Ion::Events::ShiftRight) {
    selectLeftRight(event == Ion::Events::ShiftLeft, false);
    return true;
  }
  if (event == Ion::Events::ShiftUp ||event == Ion::Events::ShiftDown) {
    selectUpDown(event == Ion::Events::ShiftUp);
    return true;
  }
  if (event == Ion::Events::Left) {
    if (contentView()->resetSelection()) {
      return true;
    }
    return TextInput::moveCursorLeft();
  }
  if (event == Ion::Events::Right) {
    if (contentView()->resetSelection()) {
      return true;
    }
    return TextInput::moveCursorRight();
  }

  if (event.hasText()) {
    return handleEventWithText(event.text());
  }
  if (event == Ion::Events::EXE) {
    return handleEventWithText("\n");
  }
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    if (contentView()->selectionIsEmpty()) {
      return false;
    }
    const char * start = contentView()->selectionStart();
    Clipboard::sharedClipboard()->store(start, contentView()->selectionEnd() - start);
    if (event == Ion::Events::Cut) {
      deleteSelection();
    }
    return true;
  }
  if (event == Ion::Events::Paste) {
    return handleEventWithText(Clipboard::sharedClipboard()->storedText(), false, true);
  }

  // The following events need a scrollToCursor and return true
  if (event == Ion::Events::Backspace) {
    if (contentView()->selectionIsEmpty()) {
      if (!removePreviousGlyph()) {
        return false;
      }
    } else {
      deleteSelection();
      return true;
    }
  } else if (event == Ion::Events::Up) {
    contentView()->resetSelection();
    contentView()->moveCursorGeo(0, -1);
  } else if (event == Ion::Events::Down) {
    contentView()->resetSelection();
    contentView()->moveCursorGeo(0, 1);
  } else if (event == Ion::Events::Clear) {
    if (!contentView()->selectionIsEmpty()) {
      deleteSelection();
      return true;
    } else if (!contentView()->removeEndOfLine()) {
      contentView()->removeStartOfLine();
    }
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

int TextArea::indentationBeforeCursor() const {
  int indentationSize = 0;
  /* Compute the number of spaces at the beginning of the line. Increase the
   * indentation size when encountering spaces, reset it to 0 when encountering
   * another code point, until reaching the beginning of the line. */
  UTF8Helper::PerformAtCodePoints(const_cast<TextArea *>(this)->contentView()->text(), ' ',
      [](int codePointOffset, void * indentationSize, int context1, int context2){
      int * castedSize = (int *) indentationSize;
      *castedSize = *castedSize + 1;
      },
      [](int codePointOffset, void * indentationSize, int context1, int context2){
      *((int *) indentationSize) = 0;
      },
      &indentationSize, 0, -1, '\n', false, cursorLocation());
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
      const char * result = UTF8Helper::CodePointAtGlyphOffset(l.text(), p.column());
      return std::min(result, l.text() + l.charLength());
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
      size_t x = UTF8Helper::GlyphOffsetAtCodePoint(l.text(), p);
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
  size_t spaceCharSize = UTF8Decoder::CharSizeOfCodePoint(' ');
  size_t spacesSize = numberOfSpaces * spaceCharSize;
  assert(location + spacesSize + sizeToMove <= m_buffer + m_bufferSize);
  memmove(location + spacesSize, location, sizeToMove);
  for (int i = 0; i < numberOfSpaces; i++) {
    UTF8Decoder::CodePointToChars(' ', location+i*spaceCharSize, (m_buffer + m_bufferSize) - location);
  }
}

CodePoint TextArea::Text::removePreviousGlyph(char * * position) {
  assert(m_buffer != nullptr);
  assert(m_buffer <= *position && *position < m_buffer + m_bufferSize);

  CodePoint removedCodePoint = 0;
  int removedSize = UTF8Helper::RemovePreviousGlyph(m_buffer, *position, &removedCodePoint);
  assert(removedSize > 0);

  // Set the new cursor position
  *position = *position - removedSize;
  return removedCodePoint;
}

size_t TextArea::Text::removeText(const char * start, const char * end) {
  assert(start <= end);
  assert(start >= m_buffer && end <= m_buffer + m_bufferSize);

  char * dst = const_cast<char* >(start);
  char * src = const_cast<char* >(end);
  size_t delta = src - dst;

  if (delta == 0) {
    return 0;
  }

  for (size_t index = src - m_buffer; index < m_bufferSize; index++) {
    *dst = *src;
    if (*src == 0) {
      assert(delta > 0);
      return delta;
    }
    dst++;
    src++;
  }
  assert(false);
  return 0;
}

size_t TextArea::Text::removeRemainingLine(const char * location, int direction) {
  assert(m_buffer != nullptr);
  assert(location >= m_buffer && location <= m_buffer + m_bufferSize);
  assert(direction > 0 || location > m_buffer);
  assert(direction < 0 || location < m_buffer + m_bufferSize);

  UTF8Decoder decoder(m_buffer, location);
  const char * codePointPosition = decoder.stringPosition();
  CodePoint nextCodePoint = direction > 0 ? decoder.nextCodePoint() : decoder.previousCodePoint();
  if (direction < 0) {
    codePointPosition = decoder.stringPosition();
  }
  while (nextCodePoint != '\n'
      && ((direction > 0 && nextCodePoint != 0)
        || (direction < 0 && codePointPosition > m_buffer)))
  {
    if (direction > 0) {
      codePointPosition = decoder.stringPosition();
    }
    nextCodePoint = direction > 0 ? decoder.nextCodePoint() : decoder.previousCodePoint();
    if (direction < 0) {
      codePointPosition = decoder.stringPosition();
    }
  }

  return removeText(direction > 0 ? location : codePointPosition, direction > 0 ? codePointPosition : location);
}

/* TextArea::Text::Line */

TextArea::Text::Line::Line(const char * text) :
  m_text(text),
  m_charLength(0)
{
  if (m_text != nullptr) {
    m_charLength = UTF8Helper::CodePointSearch(text, '\n') - m_text;
  }
}

KDCoordinate TextArea::Text::Line::glyphWidth(const KDFont * const font) const {
  return font->stringSizeUntil(m_text, m_text + m_charLength).width();
}

bool TextArea::Text::Line::contains(const char * c) const {
  return (c >= m_text)
    && ((c < m_text + m_charLength)
        || (c == m_text + m_charLength
          && (UTF8Helper::CodePointIs(c, 0)
            || UTF8Helper::CodePointIs(c, '\n')))) ;
}

/* TextArea::Text::LineIterator */

TextArea::Text::LineIterator & TextArea::Text::LineIterator::operator++() {
  const char * last = m_line.text() + m_line.charLength();
  assert(UTF8Helper::CodePointIs(last, 0) || UTF8Helper::CodePointIs(last, '\n'));
  assert(UTF8Decoder::CharSizeOfCodePoint('\n') == 1);
  m_line = Line(UTF8Helper::CodePointIs(last, 0) ? nullptr : last + 1);
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
      drawLine(ctx, y, line.text(), line.charLength(), topLeft.column(), bottomRight.column(), m_selectionStart, m_selectionEnd);
    }
    y++;
  }
}

void TextArea::ContentView::drawStringAt(KDContext * ctx, int line, int column, const char * text, int length, KDColor textColor, KDColor backgroundColor, const char * selectionStart, const char * selectionEnd, KDColor backgroundHighlightColor) const {
  if (length < 0) {
    return;
  }
  KDSize glyphSize = m_font->glyphSize();

  bool drawSelection = selectionStart != nullptr && selectionEnd > text && selectionStart < text + length;
  KDPoint nextPoint = ctx->drawString(
    text,
    KDPoint(column*glyphSize.width(), line*glyphSize.height()),
    m_font,
    textColor,
    backgroundColor,
    drawSelection ? (selectionStart >= text ? std::min<KDCoordinate>(length, selectionStart - text) : 0) : length
  );
  if (!drawSelection) {
    return;
  }
  const char * highlightedDrawStart = std::max(selectionStart, text);
  size_t highlightedDrawLength = std::min(selectionEnd - highlightedDrawStart, length - (highlightedDrawStart - text));

  nextPoint = ctx->drawString(
    highlightedDrawStart,
    nextPoint,
    m_font,
    textColor,
    backgroundHighlightColor,
    highlightedDrawLength);

  const char * notHighlightedDrawStart = highlightedDrawStart + highlightedDrawLength;
  ctx->drawString(
    notHighlightedDrawStart,
    nextPoint,
    m_font,
    textColor,
    backgroundColor,
    length - (notHighlightedDrawStart - text));
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

void TextArea::ContentView::setText(char * textBuffer, size_t textBufferSize) {
  m_text.setText(textBuffer, textBufferSize);
  m_cursorLocation = text();
}

bool TextArea::ContentView::insertTextAtLocation(const char * text, char * location) {
  int textSize = strlen(text);
  if (m_text.textLength() + textSize >= m_text.bufferSize() || textSize == 0) {
    return false;
  }
  bool lineBreak = false;

  // Scan for \n and 0
  const char * nullLocation = UTF8Helper::PerformAtCodePoints(
      text, '\n',
      [](int codePointOffset, void * lineBreak, int context1, int context2) {
        *((bool *)lineBreak) = true;
      },
      [](int c1, void * c2, int c3, int c4) { },
      &lineBreak, 0);

  assert(UTF8Helper::CodePointIs(nullLocation, 0));
  m_text.insertText(text, nullLocation - text, location);
  // Replace System parentheses (used to keep layout tree structure) by normal parentheses
  Poincare::SerializationHelper::ReplaceSystemParenthesesByUserParentheses(location, nullLocation - text);
  reloadRectFromPosition(location, lineBreak);
  return true;
}

bool TextArea::ContentView::removePreviousGlyph() {
  if (cursorLocation() <= text()) {
    assert(cursorLocation() == text());
    return false;
  }
  bool lineBreak = false;
  char * cursorLoc = const_cast<char *>(cursorLocation());
  lineBreak = m_text.removePreviousGlyph(&cursorLoc) == '\n';
  setCursorLocation(cursorLoc); // Update the cursor
  layoutSubviews(); // Reposition the cursor
  reloadRectFromPosition(cursorLocation(), lineBreak);
  return true;
}

bool TextArea::ContentView::removeEndOfLine() {
  size_t removedLine = m_text.removeRemainingLine(cursorLocation(), 1);
  if (removedLine > 0) {
    layoutSubviews();
    reloadRectFromPosition(cursorLocation(), false);
    return true;
  }
  return false;
}

bool TextArea::ContentView::removeStartOfLine() {
  if (cursorLocation() <= text()) {
    assert(cursorLocation() == text());
    return false;
  }
  size_t removedLine = m_text.removeRemainingLine(cursorLocation(), -1);
  if (removedLine > 0) {
    assert(cursorLocation() >= text() + removedLine);
    setCursorLocation(cursorLocation() - removedLine);
    reloadRectFromPosition(cursorLocation(), true);
    return true;
  }
  return false;
}

size_t TextArea::ContentView::deleteSelection() {
  assert(!selectionIsEmpty());
  size_t removedLength = m_text.removeText(m_selectionStart, m_selectionEnd);
  /* We cannot call resetSelection() because m_selectionStart and m_selectionEnd
   * are invalid */
  m_selectionStart = nullptr;
  m_selectionEnd = nullptr;
  return removedLength;
}

KDRect TextArea::ContentView::glyphFrameAtPosition(const char * text, const char * position) const {
  assert(text == m_text.text());
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
  (void) found;

  return KDRect(
    x,
    p.line() * glyphSize.height(),
    glyphSize.width(),
    glyphSize.height()
  );
}

void TextArea::ContentView::moveCursorGeo(int deltaX, int deltaY) {
  Text::Position p = m_text.positionAtPointer(cursorLocation());
  setCursorLocation(m_text.pointerAtPosition(Text::Position(p.column() + deltaX, p.line() + deltaY)));
}

void TextArea::selectUpDown(bool up) {
  const char * previousCursorLocation = contentView()->cursorLocation();
  contentView()->moveCursorGeo(0, up ? -1 : 1);
  const char * newCursorLocation = contentView()->cursorLocation();
  contentView()->addSelection(up ? newCursorLocation : previousCursorLocation, up ? previousCursorLocation : newCursorLocation);
  scrollToCursor();
}
