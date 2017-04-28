#include <escher/scroll_view_delegate.h>

ScrollViewDelegate::ScrollViewDelegate() :
  m_offset(KDPointZero)
{
}

KDPoint ScrollViewDelegate::offset() const {
  return m_offset;
}

bool ScrollViewDelegate::setOffset(KDPoint offset) {
  if (offset != m_offset) {
    m_offset = offset;
    return true;
  }
  return false;
}

