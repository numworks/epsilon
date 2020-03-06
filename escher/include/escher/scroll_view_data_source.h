#ifndef ESCHER_SCROLL_VIEW_DATA_SOURCE_H
#define ESCHER_SCROLL_VIEW_DATA_SOURCE_H

#include <kandinsky.h>

class ScrollViewDataSource;

class ScrollViewDelegate {
public:
  virtual void scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) = 0;
};

class ScrollViewDataSource {
public:
  ScrollViewDataSource() : m_delegate(nullptr), m_offset(KDPointZero) {}
  KDPoint offset() const { return m_offset; }
  bool setOffset(KDPoint offset);
  void setScrollViewDelegate(ScrollViewDelegate * delegate) {
    m_delegate = delegate;
  }
private:
  ScrollViewDelegate * m_delegate;
  KDPoint m_offset;
};

#endif
