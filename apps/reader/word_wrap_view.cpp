
#include "word_wrap_view.h"
#include "utility.h"
#include <poincare/expression.h>
#include "../shared/poincare_helpers.h"
#include <poincare/undefined.h>

namespace Reader
{

WordWrapTextView::WordWrapTextView() : 
  PointerTextView(GlobalPreferences::sharedGlobalPreferences()->font()),
  m_pageOffset(0),
  m_nextPageOffset(0),
  m_length(0),
  m_isRichTextFile(false) // Value isn't important, it will change when the file is loaded
{
  
}

void WordWrapTextView::nextPage() {
  if(m_nextPageOffset >= m_length) {
    return;
  }
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

  const int charWidth = m_font->glyphSize().width();
  const int charHeight = m_font->glyphSize().height();

  const char * endOfWord = text() + m_pageOffset - 1;
  const char * startOfWord = UTF8Helper::BeginningOfWord(text(), endOfWord);

  KDSize textSize = KDSizeZero;

  KDPoint textEndPosition(m_frame.width() - k_margin, m_frame.height() - k_margin);

  while(startOfWord>=text()) {
    startOfWord = UTF8Helper::BeginningOfWord(text(), endOfWord);
    endOfWord = UTF8Helper::EndOfWord(startOfWord);

    if (*startOfWord == '%') {
      if (updateTextColorBackward(startOfWord)) {
        endOfWord = startOfWord - 1;
        continue;
      }
    }
    if (*startOfWord == '$' && *(endOfWord-1) == '$') {
      const int wordMaxLength = 128;
      char word[wordMaxLength];
      stringNCopy(word, wordMaxLength, startOfWord + 1, endOfWord-startOfWord-2);
      Poincare::Expression expr = Poincare::Expression::Parse(word, nullptr);
      if (expr.isUninitialized()) {
        expr = Poincare::Undefined::Builder();
      }
      Poincare::Layout layout = Shared::PoincareHelpers::CreateLayout(expr);
      textSize = layout.layoutSize();
      
    }
    else {
      textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
    }
    KDPoint textStartPosition = KDPoint(textEndPosition.x()-textSize.width(), textEndPosition.y());

    if(textStartPosition.x() < k_margin) {
      textEndPosition = KDPoint(m_frame.width() - k_margin, textEndPosition.y() - charHeight);
      textStartPosition = KDPoint(textEndPosition.x() - textSize.width(), textEndPosition.y());
    }
    if(textEndPosition.y() - textSize.height() < k_margin) {
      break;
    }

    --startOfWord;
    while(startOfWord >= text() && (*startOfWord == ' ' || *startOfWord == '\n')) {
      if(*startOfWord == ' ') {
        textStartPosition = KDPoint(textStartPosition.x() - charWidth, textStartPosition.y());
      }
      else {
        textStartPosition = KDPoint(m_frame.width() - k_margin, textStartPosition.y() - charHeight);
      }
      --startOfWord;
    }

    if(textStartPosition.y() < k_margin) { // If out of page, quit
      break;
    }

    if(textStartPosition.y() != textEndPosition.y()) { // If line changed, x is at start of line 
      textStartPosition = KDPoint(m_frame.width() - k_margin, textStartPosition.y());
    }
    if(textStartPosition.x() < k_margin) { // Go to line if left overflow
      textStartPosition = KDPoint(m_frame.width() - k_margin, textStartPosition.y() - charHeight);
    }

    textEndPosition = textStartPosition;
    endOfWord = startOfWord + 1;
  }
  if(startOfWord + 1 == text()) {
    m_pageOffset = 0;
  }
  else {
    m_pageOffset = UTF8Helper::EndOfWord(startOfWord) - text() + 1;
  }
  markRectAsDirty(bounds());
}

void WordWrapTextView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), m_backgroundColor);

  const char * endOfFile = text() + m_length;
  const char * startOfWord = text() + m_pageOffset;
  const char * endOfWord = EndOfPrintableWord(startOfWord, endOfFile);
  KDPoint textPosition(k_margin, k_margin);

  const int wordMaxLength = 128;
  char word[wordMaxLength];

  Poincare::Layout layout;

  enum class ToDraw {
    Text,
    Expression,
    Nothing
  };

  ToDraw toDraw = ToDraw::Text;

  const int charWidth = m_font->glyphSize().width();
  const int charHeight = m_font->glyphSize().height();

  int nextLineOffset = charHeight;

  KDSize textSize = KDSizeZero;


  while(startOfWord < endOfFile) {

    if (*startOfWord == '%') { // Look for color keyword (ex '%bl%')
      if (updateTextColorForward(startOfWord)) {
        startOfWord = endOfWord + 1;
        endOfWord = EndOfPrintableWord(startOfWord, endOfFile);
        continue;
      }
    }
    
    if (*startOfWord == '$' && *(endOfWord-1) == '$') { // Look for expression
      stringNCopy(word, wordMaxLength, startOfWord + 1, endOfWord-startOfWord-2);
      Poincare::Expression expr = Poincare::Expression::Parse(word, nullptr);
      if (expr.isUninitialized()) {
        expr = Poincare::Undefined::Builder();
      }
      layout = Shared::PoincareHelpers::CreateLayout(expr);
      textSize = layout.layoutSize();
      toDraw = ToDraw::Expression;
    }
    else {
      textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
      stringNCopy(word, wordMaxLength, startOfWord, endOfWord-startOfWord);
      toDraw = ToDraw::Text;
    }

    KDPoint nextTextPosition = KDPoint(textPosition.x()+textSize.width(), textPosition.y());
    
    if(nextTextPosition.x() > m_frame.width() - k_margin) { // Right overflow
      textPosition = KDPoint(k_margin, textPosition.y() + nextLineOffset);
      nextTextPosition = KDPoint(k_margin + textSize.width(), textPosition.y());
      nextLineOffset = charHeight;
    }
    if (nextLineOffset < textSize.height()) {
      nextLineOffset = textSize.height();
    }

    if(textPosition.y() + textSize.height() > m_frame.height() - k_margin) { // Bottom overflow
      break;
    }

    if (toDraw == ToDraw::Expression) {
      layout.draw(ctx, textPosition, m_textColor);
    }
    else if (toDraw == ToDraw::Text) {
      ctx->drawString(word, textPosition, m_font, m_textColor, m_backgroundColor);
    }

    while(*endOfWord == ' ' || *endOfWord == '\n') {
      if(*endOfWord == ' ') {
        nextTextPosition = KDPoint(nextTextPosition.x() + charWidth, nextTextPosition.y());
      }
      else {
        nextTextPosition = KDPoint(k_margin, nextTextPosition.y() + nextLineOffset);
        nextLineOffset = charHeight;
      }
      ++endOfWord;
    }

    //We must change value of startOfWord now to avoid having
    //two times the same word if the break below is used
    startOfWord = endOfWord;

    if (endOfWord >= endOfFile) {
      break;
    }

    if(nextTextPosition.y() + textSize.height() > m_frame.height() - k_margin) { // If out of page, quit
      break;
    }
    if(nextTextPosition.y() != textPosition.y()) { // If line changed, x is at start of line 
      nextTextPosition = KDPoint(k_margin, nextTextPosition.y());
    }
    if(nextTextPosition.x() > m_frame.width() - k_margin) { // Go to line if right overflow
      nextTextPosition = KDPoint(k_margin, nextTextPosition.y() + nextLineOffset);
      nextLineOffset = charHeight;
    }

    textPosition = nextTextPosition;
    endOfWord = EndOfPrintableWord(startOfWord+1, endOfFile);
  }

  m_nextPageOffset = startOfWord - text();
};

BookSave WordWrapTextView::getBookSave() const {
  return {
    m_pageOffset,
    m_textColor
  };
}

void WordWrapTextView::setBookSave(BookSave save) {
  m_pageOffset = save.offset;
  m_textColor = save.color;
}

bool WordWrapTextView::updateTextColorForward(const char * colorStart) const {

  if (*(colorStart + 1) == '\\') {
    m_textColor = Palette::PrimaryText;
    return (*(colorStart + 3) == '%' || *(colorStart + 4) == '%');
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
        if (*(colorStart+2) == 'l') {
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

  if (*(colorStart++) != '\\') {
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
        if (*(colorStart+2) == 'l') {
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
