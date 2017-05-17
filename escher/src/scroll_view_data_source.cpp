#include <escher/scroll_view_data_source.h>

ScrollViewDataSource::ScrollViewDataSource() :
  m_offset(KDPointZero)
{
}

KDPoint ScrollViewDataSource::offset() const {
  return m_offset;
}

bool ScrollViewDataSource::setOffset(KDPoint offset) {
  if (offset != m_offset) {
    m_offset = offset;
    return true;
  }
  return false;
}

