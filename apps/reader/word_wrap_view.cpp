#include "word_wrap_view.h"

#include "utility.h"

namespace reader
{

void WordWrapTextView::nextPage()
{
    if(m_nextPageOffset >= m_length)
        return;
    m_pageOffset = m_nextPageOffset;
    markRectAsDirty(bounds());
}

void WordWrapTextView::setText(const char* text, int length)
{
    PointerTextView::setText(text);
    m_length = length;
}

void WordWrapTextView::previousPage()
{
    if(m_pageOffset <= 0)
        return;

    const int spaceWidth = m_font->stringSize(" ").width();

    const char * endOfWord = text() + m_pageOffset;
    const char * startOfWord = UTF8Helper::BeginningOfWord(text(), endOfWord);
    
    KDPoint textPosition(m_frame.width() - margin, m_frame.height() - margin);

    while(startOfWord>=text())
    {
        endOfWord = UTF8Helper::EndOfWord(startOfWord);
        KDSize textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
        KDPoint previousTextPosition = KDPoint(textPosition.x()-textSize.width(), textPosition.y());
        
        if(previousTextPosition.x() < margin)
        {
            textPosition = KDPoint(m_frame.width() - margin, textPosition.y() - textSize.height());
            previousTextPosition = KDPoint(textPosition.x() - textSize.width(), textPosition.y());
        }
        if(textPosition.y() - textSize.height() < margin)
        {
            break;
        }

        
        --startOfWord;
        while(startOfWord >= text() && *startOfWord == ' ')
        {
            previousTextPosition = KDPoint(previousTextPosition.x() - spaceWidth, previousTextPosition.y());
            --startOfWord;
        }
        if(previousTextPosition.x() < margin)
        {
            previousTextPosition = KDPoint(m_frame.width() - margin, previousTextPosition.y() - textSize.height());
        }
        
    
        while(startOfWord >= text() && *startOfWord == '\n')
        {
          previousTextPosition = KDPoint(m_frame.width() - margin, previousTextPosition.y() - textSize.height());
          --startOfWord;
        }    

        if(previousTextPosition.y() - textSize.height() < margin)
        {
            break;
        }

        textPosition = previousTextPosition;
        endOfWord = startOfWord;
        startOfWord = UTF8Helper::BeginningOfWord(text(), endOfWord);
    }
    if(startOfWord == text())
        m_pageOffset = 0;
    else
        m_pageOffset = UTF8Helper::EndOfWord(startOfWord) - text() + 1;
    markRectAsDirty(bounds());
}

void WordWrapTextView::drawRect(KDContext * ctx, KDRect rect) const
{
     ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), m_backgroundColor);

    const char * endOfFile = text() + m_length;
    const char * startOfWord = text() + m_pageOffset;
    const char * endOfWord = UTF8Helper::EndOfWord(startOfWord);
    KDPoint textPosition(margin, margin);

    const int wordMaxLength = 128;
    char word[wordMaxLength];

    const int spaceWidth = m_font->stringSize(" ").width();

    while(startOfWord < endOfFile)
    {

        KDSize textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
        KDPoint nextTextPosition = KDPoint(textPosition.x()+textSize.width(), textPosition.y());
        
        if(nextTextPosition.x() > m_frame.width() - margin)
        {
            textPosition = KDPoint(margin, textPosition.y() + textSize.height());
            nextTextPosition = KDPoint(margin + textSize.width(), textPosition.y());
        }
        if(textPosition.y() + textSize.height() > m_frame.height() - margin)
        {
            break;
        }

        stringNCopy(word, wordMaxLength, startOfWord, endOfWord-startOfWord);
        ctx->drawString(word, textPosition, m_font, m_textColor, m_backgroundColor);

        while(*endOfWord == ' ')
        {
            nextTextPosition = KDPoint(nextTextPosition.x() + spaceWidth, nextTextPosition.y());
            ++endOfWord;
        }
        if(nextTextPosition.x() > m_frame.width() - margin)
        {
            nextTextPosition = KDPoint(margin, nextTextPosition.y() + textSize.height());
        }

        while(*endOfWord == '\n')
        {
          nextTextPosition = KDPoint(margin, nextTextPosition.y() + textSize.height());
          ++endOfWord;
        }    

        if(nextTextPosition.y() + textSize.height() > m_frame.height() - margin)
        {
            break;
        }

        textPosition = nextTextPosition;
        startOfWord = endOfWord;
        endOfWord = UTF8Helper::EndOfWord(startOfWord);
    }
    m_nextPageOffset = startOfWord - text();
}

int WordWrapTextView::getPageOffset() const
{
    return m_pageOffset;
}

void WordWrapTextView::setPageOffset(int o)
{
    m_pageOffset = o;
}

}