#ifndef ESCHER_SCROLL_VIEW_DATA_SOURCE_H
#define ESCHER_SCROLL_VIEW_DATA_SOURCE_H

#include <kandinsky/point.h>

namespace Escher {

class ScrollViewDataSource;

class ScrollViewDelegate {
 public:
  virtual void scrollViewDidChangeOffset(
      ScrollViewDataSource* scrollViewDataSource) = 0;
  virtual bool updateBarIndicator(bool vertical, bool* visible) {
    return false;
  }
};

class ScrollViewDataSource {
  friend class ScrollView;

 public:
  ScrollViewDataSource() : m_delegate(nullptr), m_offset(KDPointZero) {}
  ScrollViewDelegate* delegate() const { return m_delegate; }
  KDPoint offset() const { return m_offset; }
  void setScrollViewDelegate(ScrollViewDelegate* delegate) {
    m_delegate = delegate;
  }

 private:
  bool setOffset(KDPoint offset);
  ScrollViewDelegate* m_delegate;
  KDPoint m_offset;
};

}  // namespace Escher
#endif
