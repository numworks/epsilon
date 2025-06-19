#include "layout_span_decoder.h"

namespace Poincare::Internal {

void LayoutSpanDecoder::next() {
  // next will be called when m_length is 0 but normally only once
  if (m_length > 0) {
    assert(m_layout != nullptr);
    m_layout = static_cast<const Layout*>(m_layout->nextTree());
    m_position++;
    m_length--;
  }
}

}  // namespace Poincare::Internal
