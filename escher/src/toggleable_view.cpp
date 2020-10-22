#include <escher/toggleable_view.h>


namespace Escher {

void ToggleableView::setState(bool state) {
  m_state = state;
  markRectAsDirty(bounds());
}

}
