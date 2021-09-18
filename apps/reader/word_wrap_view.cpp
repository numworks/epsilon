
#include <poincare/preferences.h>
#include "word_wrap_view.h"
#include "utility.h"

namespace Reader
{

void WordWrapTextView::nextPage() {
  if(m_nextPageOffset >= m_length) {
    return;
  }
  m_pageOffset = m_nextPageOffset;
  markRectAsDirty(bounds());
}

void WordWrapTextView::setText(const char* text, int length) {
  PointerTextView::setText(text);
  m_length = length;
}

void WordWrapTextView::previousPage() {
  if(m_pageOffset <= 0) {
    return;
  }

  const int charWidth = m_font->glyphSize().width();
  const int charHeight = m_font->glyphSize().height();

  const char * endOfWord = text() + m_pageOffset - 1;
  const char * startOfWord = UTF8Helper::BeginningOfWord(text(), endOfWord);

  KDPoint textEndPosition(m_frame.width() - k_margin, m_frame.height() - k_margin);

  while(startOfWord>=text()) {
    startOfWord = UTF8Helper::BeginningOfWord(text(), endOfWord);
    endOfWord = UTF8Helper::EndOfWord(startOfWord); 
    KDSize textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
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
  const char * endOfWord = UTF8Helper::EndOfWord(startOfWord);
  KDPoint textPosition(k_margin, k_margin);

  const int wordMaxLength = 128;
  char word[wordMaxLength];

  const int charWidth = m_font->glyphSize().width();
  const int charHeight = m_font->glyphSize().height();

  while(startOfWord < endOfFile) {
    KDSize textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
    KDPoint nextTextPosition = KDPoint(textPosition.x()+textSize.width(), textPosition.y());
    
    if(nextTextPosition.x() > m_frame.width() - k_margin) { // Right overflow
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
    if(nextTextPosition.x() > m_frame.width() - k_margin) { // Go to line if right overflow
      nextTextPosition = KDPoint(k_margin, nextTextPosition.y() + textSize.height());
    }

    textPosition = nextTextPosition;
    endOfWord = UTF8Helper::EndOfWord(startOfWord);
  }

  m_nextPageOffset = startOfWord - text();
};

int WordWrapTextView::getPageOffset() const {
  return m_pageOffset;
}

void WordWrapTextView::setPageOffset(int o) {
  m_pageOffset = o;
}

}