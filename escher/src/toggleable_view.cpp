#include <escher/toggleable_view.h>

namespace Escher {

void ToggleableView::setState(bool state) {
  if (m_state != state) {
    m_state = state;
    markRectAsDirty(bounds());
  }
}

}  // namespace Escher
