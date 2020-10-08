#include <escher/toggleable_view.h>

void ToggleableView::setState(bool state) {
  m_state = state;
  markRectAsDirty(bounds());
}