#include "hideable_even_odd_buffer_text_cell.h"

namespace Shared {

KDColor HideableEvenOddBufferTextCell::backgroundColor() const {
  if (hidden()) {
    return hideColor();
  }
  return EvenOddBufferTextCell::backgroundColor();
}

void HideableEvenOddBufferTextCell::setHide(bool hide) {
  if (hidden() != hide) {
    Hideable::setHide(hide);
    m_bufferTextView.setBackgroundColor(backgroundColor());
    reloadCell();
  }
}

}
