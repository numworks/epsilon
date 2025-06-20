#include "layout_span_decoder.h"

namespace Poincare::Internal {

void LayoutSpanDecoder::next() {
#if ASSERTIONS
  // next will be called when m_length is 0 but normally only once
  assert(m_length != k_outOfRange);
  if (m_length == 0) {
    m_length = k_outOfRange;
    return;
  }
#endif

  if (m_length > 0) {
    assert(m_layout != nullptr);
    m_layout = static_cast<const Layout*>(m_layout->nextTree());
    m_position++;
    m_length--;
  }
}

}  // namespace Poincare::Internal
