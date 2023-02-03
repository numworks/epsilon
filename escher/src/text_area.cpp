#include <escher/text_area.h>
#include <escher/clipboard.h>
#include <escher/text_input_helpers.h>
#include <ion/events.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <ion/keyboard/layout_events.h>
#include <poincare/serialization_helper.h>

#include <stddef.h>
#include <assert.h>
#include <limits.h>
#include <algorithm>

namespace Escher {
/* TextArea */

TextArea::TextArea(Responder * parentResponder, View * contentView, KDFont::Size font) :
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
  int addedTextLength = strlen(text);
  size_t previousTextLength = contentView()->getText()->textLength();
  char * insertionPosition = const_cast<char *>(cursorLocation());
  const char * textAreaBuffer = contentView()->text();
  if (indentation) {
    // Compute the indentation
    spacesCount = indentationBeforeCursor();
    if (insertionPosition > textAreaBuffer && UTF8Helper::PreviousCodePointIs(textAreaBuffer, insertionPosition, ':')) {
      spacesCount += k_indentationSpaces;
    }
    // Check the text will not overflow the buffer
    totalIndentationSize = UTF8Helper::CountOccurrences(text, '\n') * spacesCount;
    if (previousTextLength + addedTextLength + totalIndentationSize >= contentView()->getText()->bufferSize()) {
      return false;
    }
  }

  /* KDCoordinate is a int16. We must limit the number of characters per line,
   * and lines per scripts, otherwise the line rects or content rect
   * height/width can overflow int16, which results in weird visual effects.*/
  // 1 - Number of Characters per line :
  if (previousTextLength + addedTextLength > k_maxLineChars) {
    /* Only check for long lines in long scripts. PreviousTextLength and
     * addedTextLength being greater than the actual number of glyphs is not an
     * issue here. After insertion, text buffer will have this structure :
     * ".../n"+"before"+"inserted1"+("/n.../n")?+"inserted2"+"after"+"\n..."
     * Lengths :  b         ib                      ia          a
     * As maxBufferSize is lower than k_maxLineChars, there is no need to check
     * for inserted lines between "\n...\n" */
    static_assert(TextField::MaxBufferSize() < k_maxLineChars, "Pasting text might cause content rect overflow.");

    // Counting line text lengths before and after insertion.
    int b = 0;
    int a = 0;
    UTF8Helper::CountGlyphsInLine(textAreaBuffer, &b, &a, insertionPosition);

    if (a + b + addedTextLength > k_maxLineChars) {
      /* Overflow expected, depending on '/n' code point presence and position.
       * Counting : Glyphs inserted before first '/n' : ib
       *            Glyphs inserted after last '/n' : ia
       *            Number of '/n' : n */
      int glyphCount[3] = {0, 0, 0};
      UTF8Helper::PerformAtCodePoints(text, '\n',
        [](int, void * intArray, int, int) {
          // '\n' found, Increment n
          int * n = (int *)intArray + 2;
          *n = *n + 1;
          // Reset ia
          int * ia = (int *)intArray + 1;
          *ia = 0;
        },
        [](int, void * intArray, int, int) {
          if (((int *)intArray)[2] == 0) {
            // While no '\n' found, increment ib
            int * ib = (int *)intArray;
            *ib = *ib + 1;
          } else {
            // Increment ia
            int * ia = (int *)intArray + 1;
            *ia = *ia + 1;
          }
        },
        &glyphCount, 0, 0);
      // Insertion is not possible if one of the produced line is too long.
      if ((glyphCount[2] == 0 && a + glyphCount[0] + b > k_maxLineChars) || b + glyphCount[0] > k_maxLineChars || a + glyphCount[1] > k_maxLineChars) {
        return false;
      }
    }
  }

  // 2 - Total number of line :
  if (previousTextLength + addedTextLength > k_maxLines && contentView()->getText()->textLineTotal() + UTF8Helper::CountOccurrences(text, '\n') > k_maxLines) {
    // Only check for overflowed lines in long scripts to save computation
    return false;
  }

  // Insert the text
  if (!insertTextAtLocation(text, insertionPosition)) {
    return true;
  }
  assert(UTF8Decoder::CharSizeOfCodePoint(UCodePointSystem) == 1);
  /* Code point system are removed when text is inserted so the text length
   * must be updated. */
  addedTextLength = addedTextLength - UTF8Helper::CountOccurrences(text, UCodePointSystem);

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
        insertionPosition + addedTextLength);
  }
  const char * endOfInsertedText = insertionPosition + addedTextLength + totalIndentationSize;
  size_t cursorIndexInCommand = TextInputHelpers::CursorIndexInCommand(insertionPosition, endOfInsertedText);

  // Remove the Empty code points
  UTF8Helper::RemoveCodePoint(insertionPosition, UCodePointEmpty, &endOfInsertedText, endOfInsertedText);

  // Set the cursor location
  /* In theory, we should also update cursorIndexInCommand after removing
   * the empty code points. But in practice, we never need to remove empty code
   * points before a cursor we want to keep. */
  assert(forceCursorRightOfText || !UTF8Helper::HasCodePoint(insertionPosition, UCodePointEmpty, insertionPosition + cursorIndexInCommand - 1));
  const char * nextCursorLocation = forceCursorRightOfText ? endOfInsertedText : insertionPosition + cursorIndexInCommand;
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
  int step = Ion::Events::longPressFactor();
  if (event == Ion::Events::ShiftLeft || event == Ion::Events::ShiftRight) {
    selectLeftRight(event == Ion::Events::ShiftLeft, false, step);
    return true;
  }
  if (event == Ion::Events::ShiftUp || event == Ion::Events::ShiftDown) {
    selectUpDown(event == Ion::Events::ShiftUp ? OMG::VerticalDirection::Up() : OMG::VerticalDirection::Down(), step);
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    if (contentView()->resetSelection()) {
      return true;
    }
    return (event == Ion::Events::Left) ?
      TextInput::moveCursorLeft(step) :
      TextInput::moveCursorRight(step);
  }
  char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
  size_t eventTextLength = Ion::Events::copyText(static_cast<uint8_t>(event), buffer, Ion::Events::EventData::k_maxDataSize);
  if (eventTextLength > 0) {
    return handleEventWithText(buffer);
  }
  if (event == Ion::Events::EXE) {
    return handleEventWithText("\n");
  }
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    if (contentView()->selectionIsEmpty()) {
      return false;
    }
    const char * start = contentView()->selectionStart();
    Escher::Clipboard::SharedClipboard()->store(start, contentView()->selectionEnd() - start);
    if (event == Ion::Events::Cut) {
      deleteSelection();
    }
    return true;
  }
  if (event == Ion::Events::Paste) {
    return handleEventWithText(Clipboard::SharedClipboard()->storedText(), false, true);
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
  } else if (event == Ion::Events::Up || event == Ion::Events::Down) {
    contentView()->resetSelection();
    contentView()->moveCursorGeo(0, event == Ion::Events::Up ? -step : step);
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
  // assert the text to insert does not overlap the location where to insert
  assert(s >= location || s + textLength < location);

  /* The text to insert might be located after the insertion location, in which
   * case we cannot simply do a memmove, as s will be shifted by the copy. */
  bool noShift = (s + textLength < location) || (s > m_buffer + m_bufferSize);
  size_t sizeToMove = strlen(location) + 1;
  assert(location + textLength + sizeToMove <= m_buffer + m_bufferSize);
  memmove(location + textLength, location, sizeToMove);
  memmove(location, s + (noShift ? 0 : textLength), textLength);
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
  int removedSize = 0;
  if (UTF8Helper::PreviousCodePoint(m_buffer, *position) == '\n') {
    // See comments in handleEventWithText about max number of glyphs per line
    removedCodePoint = '\n';
    // removeText will handle max number of glyphs per line
    removedSize = removeText(*position-1, *position);
  } else {
    removedSize = UTF8Helper::RemovePreviousGlyph(m_buffer, *position, &removedCodePoint);
    assert(removedSize > 0);
  }
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

  /* Removing text can increase line length. See comments in handleEventWithText
   * about max number of glyphs per line. */
  if (textLength() - delta >= k_maxLineChars) {
    /* Only check for line length on long enough scripts. TextLength() and delta
     * being greater than the actual number of glyphs is not an issue here. */

    // Counting text lengths between previous and last '/n' (non removed).
    int b = 0;
    int a = 0;
    UTF8Helper::CountGlyphsInLine(text(), &b, &a, start, end);

    if (a + b > k_maxLineChars) {
      // Resulting line would exceed limits, no text is removed
      // TODO error message: Add Message to explain failure to remove text
      return 0;
    }
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

KDCoordinate TextArea::Text::Line::glyphWidth(KDFont::Size const font) const {
  return KDFont::Font(font)->stringSizeUntil(m_text, m_text + m_charLength).width();
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

KDSize TextArea::Text::span(KDFont::Size const font) const {
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
  return KDSize(width, numberOfLines * KDFont::GlyphHeight(font));
}

/* TextArea::ContentView */

void TextArea::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  // TODO: We're clearing areas we'll draw text over. It's not needed.
  clearRect(ctx, rect);

  KDSize glyphSize = KDFont::GlyphSize(m_font);

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
  KDSize glyphSize = KDFont::GlyphSize(m_font);

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
    span.width() + KDFont::GlyphWidth(m_font),
    span.height()
  );
}

void TextArea::ContentView::setText(char * textBuffer, size_t textBufferSize) {
  m_text.setText(textBuffer, textBufferSize);
  m_cursorLocation = text();
}

bool TextArea::ContentView::insertTextAtLocation(const char * text, char * location, int textLength) {
  int textLen = textLength < 0 ? strlen(text) : textLength;
  assert(textLen < 0 || textLen <= static_cast<int>(strlen(text)));
  if (m_text.textLength() + textLen >= m_text.bufferSize() || textLen == 0) {
    return false;
  }

  // Scan for \n
  bool lineBreak = UTF8Helper::HasCodePoint(text, '\n', text + textLen);

  m_text.insertText(text, textLen, location);
  // Replace System parentheses (used to keep layout tree structure) by normal parentheses
  Poincare::SerializationHelper::ReplaceSystemParenthesesAndBracesByUserParentheses(location, textLen);
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

size_t TextArea::ContentView::removeText(const char * start, const char * end) {
  return m_text.removeText(start, end);
}

size_t TextArea::ContentView::deleteSelection() {
  assert(!selectionIsEmpty());
  size_t removedLength = removeText(m_selectionStart, m_selectionEnd);
  /* We cannot call resetSelection() because m_selectionStart and m_selectionEnd
   * are invalid */
  m_selectionStart = nullptr;
  m_selectionEnd = nullptr;
  return removedLength;
}

KDRect TextArea::ContentView::glyphFrameAtPosition(const char * text, const char * position) const {
  assert(text == m_text.text());
  KDSize glyphSize = KDFont::GlyphSize(m_font);
  Text::Position p = m_text.positionAtPointer(position);

  KDCoordinate x = 0;
  bool found = false;
  int y = 0;
  for (Text::Line l : m_text) {
    if (p.line() == y) {
      x = KDFont::Font(m_font)->stringSizeUntil(l.text(), position).width();
      found = true;
      break;
    }
    y++;
  }
  assert(found);
  (void) found;

  // Check for KDCoordinate overflow
  assert(x < KDCOORDINATE_MAX - glyphSize.width() && p.line() * glyphSize.height() < KDCOORDINATE_MAX - glyphSize.height());

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

void TextArea::selectUpDown(OMG::VerticalDirection direction, int step) {
  const char * previousCursorLocation = contentView()->cursorLocation();
  contentView()->moveCursorGeo(0, direction.isUp() ? -step : step);
  const char * newCursorLocation = contentView()->cursorLocation();
  if (direction.isUp()) {
    contentView()->addSelection(newCursorLocation, previousCursorLocation);
  } else {
    contentView()->addSelection(previousCursorLocation, newCursorLocation);
  }
  scrollToCursor();
}

}
