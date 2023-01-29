
#include "word_wrap_view.h"
#include "utility.h"
#include "tex_parser.h"
#include <poincare/expression.h>
#include "../shared/poincare_helpers.h"
#include <poincare/undefined.h>
#include "read_book_controller.h"

namespace Reader
{

WordWrapTextView::WordWrapTextView(ReadBookController * readBookController) :
  PointerTextView(GlobalPreferences::sharedGlobalPreferences()->font()),
  m_pageOffset(0),
  m_nextPageOffset(0),
  m_length(0),
  m_isRichTextFile(false), // Value isn't important, it will change when the file is loaded
  m_lastPagesOffsetsIndex(0),
  m_textColor(Palette::PrimaryText),
  m_readBookController(readBookController)
{
  for (int i = 0; i < k_lastOffsetsBufferSize; i++) {
    m_lastPagesOffsets[i] = -1; // -1 Means : no informations
  }
}

void WordWrapTextView::nextPage() {
  if(m_nextPageOffset >= m_length) {
    return;
  }
  m_lastPagesOffsets[m_lastPagesOffsetsIndex] = m_pageOffset;
  m_lastPagesOffsetsIndex = (m_lastPagesOffsetsIndex + 1) % k_lastOffsetsBufferSize;
  m_pageOffset = m_nextPageOffset;
  markRectAsDirty(bounds());
}

void WordWrapTextView::setText(const char* text, int length, bool isRichTextFile) {
  PointerTextView::setText(text);
  m_length = length;
  m_isRichTextFile = isRichTextFile;
}

void WordWrapTextView::previousPage() {
  if(m_pageOffset <= 0) {
    return;
  }

  /* We check if we have available data in our buffer */
  int offsetToCheck = (m_lastPagesOffsetsIndex + k_lastOffsetsBufferSize - 1) % k_lastOffsetsBufferSize;
  if (m_lastPagesOffsets[offsetToCheck] != -1) {
    m_lastPagesOffsetsIndex = offsetToCheck;
    m_pageOffset = m_lastPagesOffsets[offsetToCheck];
    m_lastPagesOffsets[offsetToCheck] = -1;
  } else if (m_isRichTextFile) {
    richTextPreviousPage();
  } else {
    plainTextPreviousPage();
  }

  markRectAsDirty(bounds());
}

void WordWrapTextView::richTextPreviousPage() {
  const int charWidth = m_font->glyphSize().width();
  const int charHeight = m_font->glyphSize().height();

  const char * endOfWord = text() + m_pageOffset - 1;
  if (*endOfWord == '\n') {
    endOfWord --;
  }

  KDCoordinate baseline = charHeight / 2;

  KDPoint textBottomEndPosition = KDPoint(m_frame.width() - k_margin, m_frame.height() - k_margin);
  KDCoordinate lineHeight = charHeight;

  while(endOfWord >= text()) {
    // 1. Skip whitespaces and line jumps
    const char * invisiblesCharJumped = endOfWord; // We use this to update endOfWord only if we don't change page
    bool changePage = false;
    while(invisiblesCharJumped >= text() && (*invisiblesCharJumped == ' ' || *invisiblesCharJumped == '\n')) {
      if(*invisiblesCharJumped == '\n') {
        textBottomEndPosition = KDPoint(m_frame.width() - k_margin, textBottomEndPosition.y() - lineHeight);
        lineHeight = charHeight;
        baseline = charHeight / 2;
        // We check if we must change page
        if (textBottomEndPosition.y() - lineHeight <= k_margin) {
          // We don't let text on a new line or a space
          endOfWord ++;
          changePage = true;
          break;
        }
      } else {
        textBottomEndPosition = KDPoint(textBottomEndPosition.x() - charWidth, textBottomEndPosition.y());
      }
      invisiblesCharJumped--;
    }

    if (changePage) {
      break;
    }
    endOfWord = invisiblesCharJumped;

    // 3. If word is a color change
    if (*endOfWord == '%' && *(endOfWord - 1) != '\\') {
      const char * startOfWord = endOfWord - 2;
      while (*startOfWord != '%') {
        startOfWord--;
      }

      if (updateTextColorBackward(startOfWord)) {
        endOfWord = startOfWord - 1; // Update next endOfWord
        continue;
      } else {
        m_readBookController->throwError();
        return;
      }
    }

    KDSize textSize = KDSizeZero;

    // 4. If word is a mathematical expression
    if (*endOfWord == '$' && *(endOfWord - 1) != '\\') {
      // We go to the end  of the expression + 1
      const char * expressionStart = --endOfWord;
      while (*expressionStart != '$') {
        if (expressionStart < text()) {
          break; // File isn't rightly formated
        }
        expressionStart --;
      }

      TexParser parser = TexParser(expressionStart, endOfWord);
      Layout layout = parser.getLayout();

      KDCoordinate layoutBaseline = layout.baseline();

      // We check if we must change baseline
      if (layoutBaseline > baseline) {
        baseline = layoutBaseline;
      }

      KDSize layoutSize = layout.layoutSize();
      textSize = KDSize(layoutSize.width(), layoutSize.height() + baseline - layoutBaseline);

      endOfWord = expressionStart;
    }

    // 5. Else it's text
    else {
      // We go to the start of the word
      const char * startOfWord = StartOfPrintableWord(endOfWord, text());

      textSize = m_font->stringSizeUntil(startOfWord, endOfWord + 1);

      endOfWord = startOfWord;
    }

    // 6. We check if we must change line
    if (textBottomEndPosition.x() - textSize.width() <= k_margin) {
      textBottomEndPosition = KDPoint(m_frame.width() - k_margin, textBottomEndPosition.y() - lineHeight);
      lineHeight = 0;
      baseline = charHeight;
      // We will check if we must change page below
    }
    textBottomEndPosition = KDPoint(textBottomEndPosition.x() - textSize.width(), textBottomEndPosition.y());

    // 7. We update height of the line if needed
    if (textSize.height() > lineHeight) {
      lineHeight = textSize.height();
      // We check if we must change page
      if (textBottomEndPosition.y() - lineHeight <= k_margin) {
        break;
      }
    }

    endOfWord -= 1;
  }

  if (endOfWord + 1 == text()) {
      m_pageOffset = 0;
  } else {
      m_pageOffset = endOfWord - text();
  }
}

void WordWrapTextView::plainTextPreviousPage() {
  const int charWidth = m_font->glyphSize().width();
  const int charHeight = m_font->glyphSize().height();

  const char * endOfWord = text() + m_pageOffset - 1;
  const char * startOfWord = UTF8Helper::BeginningOfWord(text(), endOfWord);

  KDPoint textEndPosition(m_frame.width() - k_margin, m_frame.height() - k_margin);

  while(endOfWord>text()) {
    KDSize textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
    KDPoint textStartPosition = KDPoint(textEndPosition.x()-textSize.width(), textEndPosition.y());

    if (textStartPosition.x() < k_margin) {
      // We check if the word is too long to be displayed entirely on just one line
      if(textSize.width() > m_frame.width() - 2 * k_margin) {
        startOfWord = endOfWord - (textEndPosition.x() - k_margin) / charWidth;
        continue;
      }
      textEndPosition = KDPoint(m_frame.width() - k_margin, textEndPosition.y() - charHeight);
      textStartPosition = KDPoint(textEndPosition.x() - textSize.width(), textEndPosition.y());
    }
    if (textEndPosition.y() - textSize.height() < k_margin) {
      break;
    }

    --startOfWord;
    while (startOfWord >= text() && (*startOfWord == ' ' || *startOfWord == '\n')) {
      if (*startOfWord == ' ') {
        textStartPosition = KDPoint(textStartPosition.x() - charWidth, textStartPosition.y());
      } else {
        textStartPosition = KDPoint(m_frame.width() - k_margin, textStartPosition.y() - charHeight);
      }
      --startOfWord;
    }

    if (textStartPosition.y() < k_margin) { // If out of page, quit
      break;
    }

    if (textStartPosition.y() != textEndPosition.y()) { // If line changed, x is at start of line
      textStartPosition = KDPoint(m_frame.width() - k_margin, textStartPosition.y());
    }
    if (textStartPosition.x() <= k_margin) { // Go to line if left overflow
      textStartPosition = KDPoint(m_frame.width() - k_margin, textStartPosition.y() - charHeight);
    }

    textEndPosition = textStartPosition;
    endOfWord = startOfWord + 1;
    startOfWord = UTF8Helper::BeginningOfWord(text(), endOfWord);
  }

  while (endOfWord < text() + m_length && (*endOfWord == ' ' || *endOfWord == '\n')) {
    endOfWord++;
  }

  m_pageOffset = endOfWord - text();
}

void WordWrapTextView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), m_backgroundColor);

  if (m_isRichTextFile) {
    richTextDrawRect(ctx, rect);
  } else {
    plainTextDrawRect(ctx, rect);
  }
}

void WordWrapTextView::richTextDrawRect(KDContext * ctx, KDRect rect) const {
  enum class ToDraw {
    Text,
    Expression
  };

  bool endOfPage = false;

  const char * endOfFile = text() + m_length;
  const char * startOfWord = text() + m_pageOffset;

  const int charWidth = m_font->glyphSize().width();
  const int charHeight = m_font->glyphSize().height();

  const int wordMaxLength = (m_frame.width() - 2*k_margin )  / charWidth;
  char word[wordMaxLength];

  Layout tooBigLayout = EmptyLayout::Builder(); // To store layouts that are too big to be displayed entirely on one line
  KDCoordinate tooBigLayoutAlreadyWroteWidth = 0; // We store here the part of the layout that has already been written

  KDPoint textPosition = KDPoint(k_margin, k_margin);

  while (!endOfPage && (startOfWord < endOfFile || !tooBigLayout.isEmpty())) {
    // We process line by line

    const char * firstReadIndex = startOfWord;

    // 1. We compute the size of what we are going to draw and the baseline
    KDSize lineSize = KDSize(0, charHeight);
    KDCoordinate baseline = charHeight / 2;

    while (firstReadIndex < endOfFile) {
      // 1.0. We check if we are drawing a too big layout
      if (!tooBigLayout.isEmpty()) {
        lineSize = tooBigLayout.layoutSize();
        baseline = tooBigLayout.baseline();

        if (tooBigLayout.layoutSize().width() - tooBigLayoutAlreadyWroteWidth > m_frame.width() - 2 * k_margin) {
          // Remaining part of the layout don't fit on the line
          break;
        }
      }

      KDSize textSize = KDSizeZero;
      KDCoordinate updatedBaseline = 0; // 0 if it's not a layout

      // 1.1. And we check if we are at the end of the line
      if(*firstReadIndex == '\n') {
        break;
      }

      // 1.2. Check if we are in a color change
      if (*firstReadIndex == '%') {  // We assume each '%' non-escaped is announcing a color change
        // We go to the end of the color change + 1
        const char * startIndex = firstReadIndex;

        do {
          firstReadIndex ++;
        } while (*firstReadIndex != '%' && firstReadIndex < endOfFile && startIndex - firstReadIndex < 5);
        firstReadIndex ++;

        if (firstReadIndex - startIndex > 5) {
          m_readBookController->throwError();
          return;
        }

        continue;
      }

      // 1.3. Check if we are in a math expression
      if (*firstReadIndex == '$') {
        // We go to the end  of the expression + 1
        const char * expressionStart = ++firstReadIndex;
        while (*firstReadIndex != '$') {
          if (firstReadIndex > endOfFile) {
            break; // File isn't rightly formated
          }
          firstReadIndex ++;
        }

        TexParser parser = TexParser(expressionStart, firstReadIndex);
        Layout firstReadLayout = parser.getLayout();

        KDCoordinate layoutBaseline = firstReadLayout.baseline();

        updatedBaseline = baseline; // We really update baseline after, if the layout fit on the line
        if (layoutBaseline > baseline) {
          updatedBaseline = layoutBaseline;
        }

        KDSize layoutSize = firstReadLayout.layoutSize();
        textSize = KDSize(layoutSize.width(), layoutSize.height() + updatedBaseline - layoutBaseline);

        firstReadIndex ++;
      }

      // 1.4. Else it's text
      else {
        if ((*firstReadIndex == '\\' && *(firstReadIndex + 1) == '$') || (*firstReadIndex == '\\' && *(firstReadIndex + 1) == '%')) { // We escape '$' and '%' if needed
          firstReadIndex ++;
        }

        const char * endOfWord = EndOfPrintableWord(firstReadIndex + 1, endOfFile);

        textSize = m_font->stringSizeUntil(firstReadIndex, endOfWord);

        firstReadIndex = endOfWord;
      }

      // 1.5. We update size
      int newWidth = lineSize.width() + textSize.width();
      // We check if the new text fit on the line.
      // If not, we look if only the word cannot fit on one line. If so, we do not go to the line
      if (newWidth > m_frame.width() - 2 * k_margin && !(textSize.width() > m_frame.width() - 2 * k_margin)) {
        break;
      }

      if (updatedBaseline) { // Now we update baseline
        baseline = updatedBaseline;
      }

      int newHeight;
      if (lineSize.height() > textSize.height()) {
        newHeight = lineSize.height();
      } else {
        newHeight = textSize.height();
        // We check if all the content can be displayed
        if (textPosition.y() + newHeight > bounds().height() -  k_margin) {
          endOfPage = true;
          break;
        }
      }
      lineSize = KDSize(lineSize.width() + textSize.width(), newHeight);

      // 1.6. We go to the next word
      while (*firstReadIndex == ' ') {
        lineSize = KDSize(lineSize.width() + charWidth, lineSize.height());
        ++firstReadIndex;
      }
    }

    if (endOfPage) {
      break;
    }

    // 2. And now... we read the line again to draw it !
    while (startOfWord < endOfFile || !tooBigLayout.isEmpty()) {
      // 2.0 Before all, we check if we were drawing a layout that was too big to fit on one line.
      //     In this case, we do all the routine here.
      if (!tooBigLayout.isEmpty()) {
        KDPoint position = KDPoint(textPosition.x() - tooBigLayoutAlreadyWroteWidth, textPosition.y());
        tooBigLayout.draw(ctx, position, m_textColor, m_backgroundColor);
        // We fill the left margin
        ctx->fillRect(KDRect(0, textPosition.y(), k_margin, tooBigLayout.layoutSize().height()), m_backgroundColor);

        KDCoordinate drawnWidth = tooBigLayout.layoutSize().width() - tooBigLayoutAlreadyWroteWidth;
        tooBigLayoutAlreadyWroteWidth += drawnWidth;

        if (drawnWidth > m_frame.width() - 2 * k_margin) {
          // We have to fill the margin with the background color
          ctx->fillRect(KDRect(textPosition.x() + drawnWidth, textPosition.y(), k_margin, lineSize.height()), m_backgroundColor);
          textPosition = KDPoint(k_margin, textPosition.y() + lineSize.height());
          break;
        } else {
          tooBigLayout = EmptyLayout::Builder(); // We have finished drawing the tooBigLayout
        }
        continue;
      }

      Layout layout;

      //2.1. We check if we are at the end of the line
      if (*startOfWord == '\n') {
        startOfWord++;
        textPosition = KDPoint(k_margin, textPosition.y() + lineSize.height());
        break;
        // We aren't supposed to be at the end of the page, else the loop on top would have stopped drawing
      }

      const char * endOfWord;

      // 2.2. Check if we are in a color change
      if (*startOfWord == '%') {
        if (updateTextColorForward(startOfWord)) {
          startOfWord += 2;  // We can add at least 2 ('%' + the color first char)
          while (*startOfWord != '%') {
            startOfWord ++;
          }
          startOfWord ++;
          continue;
        }
        else {
          m_readBookController->throwError();
          return;
        }
      }

      // 2.3. Check what we are going to draw and his size

      KDSize textSize = KDSizeZero;
      ToDraw toDraw;

      // 2.3.1. Check if we are in a math expression
      if (*startOfWord == '$') {
        endOfWord = startOfWord + 1;
        while (*endOfWord != '$') {
          if (endOfWord > endOfFile) {
            break; // File isn't rightly formated
          }
          endOfWord ++;
        }
        endOfWord ++;

        TexParser parser = TexParser(startOfWord + 1, endOfWord - 1);
        layout = parser.getLayout();
        textSize = layout.layoutSize();

        toDraw = ToDraw::Expression;
      }

      // 2.3.2 Else it's text
      else {
        if ((*startOfWord == '\\' && *(startOfWord + 1) == '$') || (*startOfWord == '\\' && *(startOfWord + 1) == '%')) {
          startOfWord ++;
        }
        endOfWord = EndOfPrintableWord(startOfWord + 1, endOfFile);
        textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
        stringNCopy(word, wordMaxLength, startOfWord, endOfWord-startOfWord);
        toDraw = ToDraw::Text;
      }

      // 2.4 We decide where to draw and if we must change line
      KDPoint endTextPosition = KDPoint(textPosition.x() + textSize.width(), textPosition.y());

      // 2.4.1. Check if we need to go to the next line
      if(endTextPosition.x() > m_frame.width() - k_margin) {

        // We check if the word is too long to be displayed entirely on just one line
        if(textSize.width() > m_frame.width() - 2 * k_margin) {
          if (toDraw == ToDraw::Text) {
            startOfWord = endOfWord - (endTextPosition.x() - k_margin) / charWidth;
            continue;
          } else {
            assert(toDraw == ToDraw::Expression);
            tooBigLayout = layout;
            tooBigLayoutAlreadyWroteWidth += m_frame.width() - k_margin - textPosition.x();
            endTextPosition = KDPoint(k_margin, textPosition.y() + lineSize.height()); // We jump line now, because this will be the next value of textPosition
          }
        } else {
          textPosition = KDPoint(k_margin, textPosition.y() + lineSize.height());
          // endTextPosition will be updated later
          break;
        }
      }

      // 2.5. Now we draw !
      if (toDraw == ToDraw::Expression) {
        KDPoint position = KDPoint(textPosition.x(), textPosition.y() + baseline - layout.baseline());
        layout.draw(ctx, position, m_textColor, m_backgroundColor);

        if (!tooBigLayout.isEmpty()) {
          // We fill the margin
          ctx->fillRect(KDRect(m_frame.width() - k_margin, textPosition.y(), k_margin, lineSize.height()), m_backgroundColor);
        }
      }
      else {
        KDPoint position = KDPoint(textPosition.x(), textPosition.y() + baseline - charHeight / 2);
        ctx->drawString(word, position, m_font, m_textColor, m_backgroundColor);
      }

      // 2.6. Update the position
      textPosition = endTextPosition;

      // 2.7. And we go to the next word
      while (*endOfWord == ' ') {
        endOfWord++;
        textPosition = KDPoint(textPosition.x() + charWidth, textPosition.y());
      }
      startOfWord = endOfWord;

      // 2.8. We exit now if we are in the "too big layout" case
      if (!tooBigLayout.isEmpty()) {
        break;
      }
    }
  }
  m_nextPageOffset = startOfWord - text();
}


void WordWrapTextView::plainTextDrawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), m_backgroundColor);

  const char * endOfFile = text() + m_length;
  const char * startOfWord = text() + m_pageOffset;
  const char * endOfWord = UTF8Helper::EndOfWord(startOfWord, endOfFile);
  KDPoint textPosition(k_margin, k_margin);

  const int wordMaxLength = 128;
  char word[wordMaxLength];

  const int charWidth = m_font->glyphSize().width();
  const int charHeight = m_font->glyphSize().height();

  while(startOfWord < endOfFile) {
    KDSize textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
    KDPoint nextTextPosition = KDPoint(textPosition.x()+textSize.width(), textPosition.y());

    if(nextTextPosition.x() > m_frame.width() - k_margin) { // Right overflow
      // We check if the word is too long to be displayed entirely on just one line
      if(textSize.width() > m_frame.width() - 2 * k_margin) {
        endOfWord = startOfWord + (m_frame.width() - k_margin - textPosition.x()) / charWidth;
        continue;
      }
      textPosition = KDPoint(k_margin, textPosition.y() + textSize.height());
      nextTextPosition = KDPoint(k_margin + textSize.width(), textPosition.y());
    }

    if(textPosition.y() + textSize.height() > m_frame.height() - k_margin) { // Bottom overflow
      break;
    }

    stringNCopy(word, wordMaxLength, startOfWord, endOfWord-startOfWord);
    ctx->drawString(word, textPosition, m_font, m_textColor, m_backgroundColor);

    while(*endOfWord == ' ' || *endOfWord == '\n') {
      if(*endOfWord == ' ') {
        nextTextPosition = KDPoint(nextTextPosition.x() + charWidth, nextTextPosition.y());
      }
      else {
        nextTextPosition = KDPoint(k_margin, nextTextPosition.y() + charHeight);
      }
      ++endOfWord;
    }

    //We must change value of startOfWord now to avoid having
    //two times the same word if the break below is used
    startOfWord = endOfWord;

    if(nextTextPosition.y() + textSize.height() > m_frame.height() - k_margin) { // If out of page, quit
      break;
    }
    if(nextTextPosition.y() != textPosition.y()) { // If line changed, x is at start of line
      nextTextPosition = KDPoint(k_margin, nextTextPosition.y());
    }
    if(nextTextPosition.x() >= m_frame.width() - k_margin) { // Go to line if right overflow
      nextTextPosition = KDPoint(k_margin, nextTextPosition.y() + textSize.height());
    }

    textPosition = nextTextPosition;
    endOfWord = UTF8Helper::EndOfWord(startOfWord, endOfFile);
  }

  m_nextPageOffset = startOfWord - text();
}

BookSave WordWrapTextView::getBookSave() const {
  return {
    m_pageOffset,
    m_textColor
  };
}

void WordWrapTextView::setBookSave(BookSave save) {
  m_pageOffset = save.offset;
  // TODO: Understand why the color save crash the calculator and fix it
  // m_textColor = save.color;
  m_lastPagesOffsetsIndex = 0;

  for (int i = 0; i < k_lastOffsetsBufferSize; i++) {
    m_lastPagesOffsets[i] = -1; // -1 Means : no informations
  }
}

bool WordWrapTextView::updateTextColorForward(const char * colorStart) const {

  if (*(colorStart + 1) == '\\' && (*(colorStart + 3) == '%' || *(colorStart + 4) == '%')) {
    m_textColor = Palette::PrimaryText;
    return true;
  }

  int keySize = 1;
  KDColor lastColor = m_textColor;

  switch (*(colorStart+1))
  {
    case 'r':
        if (*(colorStart+2) == 'l') {
            m_textColor = Palette::RedLight;
            keySize = 2;
        }
        else {
            m_textColor = Palette::Red;
        }
        break;
    case 'm':
        m_textColor = Palette::Magenta;
        break;
    case 't':
        m_textColor = Palette::Turquoise;
        break;
    case 'p':
        if (*(colorStart+2) == 'k') {
            m_textColor = Palette::Pink;
            keySize = 2;
        }
        else if (*(colorStart+2) == 'p') {
            m_textColor = Palette::Purple;
            keySize = 2;
        }
        break;
    case 'b':
        if (*(colorStart+2) == 'r') {
            m_textColor = Palette::Brown;
            keySize = 2;
        }
        else if (*(colorStart+2) == 'l') {
            m_textColor = Palette::BlueLight;
            keySize = 2;
        }
        else {
            m_textColor = Palette::Blue;
        }
        break;
    case 'o':
        m_textColor = Palette::Orange;
        break;
    case 'g':
        if (*(colorStart+2) == 'l') {
            m_textColor = Palette::GreenLight;
            keySize = 2;
        }
        else {
            m_textColor = Palette::Green;
        }
        break;
    case 'c':
        m_textColor = Palette::Cyan;
        break;

    default:
        return false;
  }

  if (*(colorStart + keySize + 1) != '%') {
    m_textColor = lastColor;
    return false;
  }

  return true;
}

bool WordWrapTextView::updateTextColorBackward(const char * colorStart) const {

  if (*(++colorStart) != '\\') {
    if (*(colorStart + 1) == '%' || *(colorStart + 2) == '%') {
      m_textColor = Palette::PrimaryText;
      return true;
    }
    return false;
  }

  int keySize = 1;
  KDColor lastColor = m_textColor;
  switch (*(colorStart+1))
  {
    case 'r':
        if (*(colorStart+2) == 'l') {
            m_textColor = Palette::RedLight;
            keySize = 2;
        }
        else {
            m_textColor = Palette::Red;
        }
        break;
    case 'm':
        m_textColor = Palette::Magenta;
        break;
    case 't':
        m_textColor = Palette::Turquoise;
        break;
    case 'p':
        if (*(colorStart+2) == 'k') {
            m_textColor = Palette::Pink;
            keySize = 2;
        }
        else if (*(colorStart+2) == 'p') {
            m_textColor = Palette::Purple;
            keySize = 2;
        }
        break;
    case 'b':
        if (*(colorStart+2) == 'r') {
            m_textColor = Palette::Brown;
            keySize = 2;
        }
        else if (*(colorStart+2) == 'l') {
            m_textColor = Palette::BlueLight;
            keySize = 2;
        }
        else {
            m_textColor = Palette::Blue;
        }
        break;
    case 'o':
        m_textColor = Palette::Orange;
        break;
    case 'g':
        if (*(colorStart+2) == 'l') {
            m_textColor = Palette::GreenLight;
            keySize = 2;
        }
        else {
            m_textColor = Palette::Green;
        }
        break;
    case 'c':
        m_textColor = Palette::Cyan;
        break;

    default:
        return false;
  }

  if (*(colorStart + keySize + 1) != '%') {
    m_textColor = lastColor;
    return false;
  }

  return true;
}

}
