#ifndef _WORD_WRAP_VIEW_H_
#define _WORD_WRAP_VIEW_H_

#include <escher.h>

namespace reader
{

class WordWrapTextView : public PointerTextView {
public:
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setText(const char*, int length);
    void nextPage();
    void previousPage();
    int getPageOffset() const;
    void setPageOffset(int o);
protected:
    int m_pageOffset = 0;
    mutable int m_nextPageOffset = 0;
    int m_length = 0;

    static const int margin = 10;
};

}

#endif