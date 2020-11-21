#include "word_wrap_view.h"

#include "utility.h"

#include <iostream>

namespace reader
{
void WordWrapTextView::drawRect(KDContext * ctx, KDRect rect) const
{
     ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), m_backgroundColor);


    const char * current = text();
    const char * startOfWord = current;
    const char * endOfWord = UTF8Helper::EndOfWord(startOfWord);
    KDPoint textPosition(0, 0);

    const int wordMaxLength = 128;
    char word[wordMaxLength];

    const int spaceWidth = m_font->stringSize(" ").width();

    while(*startOfWord != 0)
    {

        KDSize textSize = m_font->stringSizeUntil(startOfWord, endOfWord);
        KDPoint nextTextPosition = KDPoint(textPosition.x()+textSize.width(), textPosition.y());
        
        if(nextTextPosition.x() > m_frame.width())
        {
            textPosition = KDPoint(0, textPosition.y() + textSize.height());
            nextTextPosition = KDPoint(textSize.width(), textPosition.y());
        }
        
        stringNCopy(word, wordMaxLength, startOfWord, endOfWord-startOfWord);
        ctx->drawString(word, textPosition, m_font, m_textColor, m_backgroundColor);

        while(*endOfWord == ' ')
        {
            nextTextPosition = KDPoint(nextTextPosition.x() + spaceWidth, nextTextPosition.y());
            ++endOfWord;
        }
        if(nextTextPosition.x() > m_frame.width())
        {
            nextTextPosition = KDPoint(0, nextTextPosition.y() + textSize.height());
        }

        while(*endOfWord == '\n')
        {
          nextTextPosition = KDPoint(0, nextTextPosition.y() + textSize.height());
          ++endOfWord;
        }    

        if(nextTextPosition.y() + textSize.height() > m_frame.height())
        {
            break;
        }

        textPosition = nextTextPosition;
        startOfWord = endOfWord;
        endOfWord = UTF8Helper::EndOfWord(startOfWord);

        std::cout<<startOfWord<<std::endl;
    }
}

}