#include <escher/blink_timer.h>

namespace Escher {

TextCursorView * BlinkTimer::s_cursor = nullptr;

void BlinkTimer::RegisterCursor(TextCursorView * cursor) {
  if (s_cursor == cursor) {
    return;
  }
  if (s_cursor) {
    s_cursor->setVisible(false);
  }
  s_cursor = cursor;
  if (s_cursor) {
    s_cursor->setVisible(true);
  }
}

void BlinkTimer::forceCursorVisible() {
  if (s_cursor) {
    s_cursor->setVisible(true);
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
