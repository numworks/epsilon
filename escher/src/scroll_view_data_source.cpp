#include <escher/scroll_view_data_source.h>

namespace Escher {

bool ScrollViewDataSource::setOffset(KDPoint offset) {
  if (offset != m_offset) {
    m_offset = offset;
    if (m_delegate) {
      m_delegate->scrollViewDidChangeOffset(this);
    }
    return true;
  }
  return false;
}

}  // namespace Escher
