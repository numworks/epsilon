#include <escher/blink_timer.h>

namespace Escher {

TextCursorView * BlinkTimer::s_cursor = nullptr;

void BlinkTimer::forceCursorVisible() {
  if (s_cursor) {
    s_cursor->forceVisible();
    reset();
  }
}

bool BlinkTimer::fire() {
  if (s_cursor) {
    s_cursor->switchVisible();
    return true;
  }
  return false;
}

}
