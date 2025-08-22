#pragma once

#include <kandinsky/point.h>

namespace Escher {

class ScrollViewDataSource;

class ScrollViewDataSourceDelegate {
 public:
  virtual void scrollViewDidChangeOffset(
      ScrollViewDataSource* scrollViewDataSource){};
  virtual bool updateBarIndicator(bool vertical, bool* visible) {
    return false;
  }
};

class ScrollViewDataSource {
 public:
  ScrollViewDataSource() : m_delegate(nullptr), m_offset(KDPointZero) {}
  ScrollViewDataSourceDelegate* delegate() const { return m_delegate; }
  KDPoint offset() const { return m_offset; }
  bool setOffset(KDPoint offset);
  void setScrollViewDataSourceDelegate(ScrollViewDataSourceDelegate* delegate) {
    m_delegate = delegate;
  }

 private:
  ScrollViewDataSourceDelegate* m_delegate;
  KDPoint m_offset;
};

}  // namespace Escher
