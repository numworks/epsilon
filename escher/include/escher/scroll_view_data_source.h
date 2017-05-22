#ifndef ESCHER_SCROLL_VIEW_DATA_SOURCE_H
#define ESCHER_SCROLL_VIEW_DATA_SOURCE_H

#include <kandinsky.h>

class ScrollViewDataSource {
public:
  ScrollViewDataSource();
  KDPoint offset() const;
  bool setOffset(KDPoint offset);
private:
  KDPoint m_offset;
};

#endif
