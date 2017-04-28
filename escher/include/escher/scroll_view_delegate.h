#ifndef ESCHER_SCROLL_VIEW_DELEGATE_H
#define ESCHER_SCROLL_VIEW_DELEGATE_H

#include <kandinsky.h>

class ScrollViewDelegate {
public:
  ScrollViewDelegate();
  KDPoint offset() const;
  bool setOffset(KDPoint offset);
private:
  KDPoint m_offset;
};

#endif
