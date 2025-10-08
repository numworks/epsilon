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
#else
  if (m_length == 0) {
    return;
  }
#endif

  assert(m_layout != nullptr);

  if (m_layout->isCombinedCodePointsLayout() && !m_isOnCombining) {
    m_isOnCombining = true;
    return;
  }

  m_layout = static_cast<const Layout*>(m_layout->nextTree());
  m_position++;
  m_length--;
  m_isOnCombining = false;
}

}  // namespace Poincare::Internal
