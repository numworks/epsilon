#include <escher/toggleable_view.h>

namespace Escher {

void Toggleable::setState(bool state) {
  if (m_state != state) {
    m_state = state;
  }
}

}  // namespace Escher
