#include <escher/blink_timer.h>

namespace Escher {

bool BlinkTimer::isActive() const { return SharedCursor()->shouldBlink(); }

KDPoint BlinkTimer::origin() const { return SharedCursor()->absoluteOrigin(); }

void BlinkTimer::forceCursorVisible() {
  if (SharedCursor()->shouldBlink()) {
    SharedCursor()->setVisible(true);
    reset();
  }
}

bool BlinkTimer::fire() {
  if (SharedCursor()->shouldBlink()) {
    SharedCursor()->switchVisible();
    return true;
  }
  return false;
}

}  // namespace Escher
