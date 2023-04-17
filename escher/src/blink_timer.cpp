#include <escher/blink_timer.h>

namespace Escher {

TextCursorView* BlinkTimer::s_cursor = nullptr;

void BlinkTimer::RegisterCursor(TextCursorView* cursor) {
  TextCursorView* newCursor = cursor;
  if (newCursor && newCursor->bounds().isEmpty()) {
    // Do not try to blink an offscreen cursor
    newCursor = nullptr;
  }
  if (s_cursor == newCursor) {
    return;
  }
  if (s_cursor) {
    s_cursor->setVisible(false);
  }
  s_cursor = newCursor;
  if (s_cursor) {
    s_cursor->setVisible(true);
  }
}

void BlinkTimer::forceCursorVisible() {
  if (s_cursor && !s_cursor->bounds().isEmpty()) {
    s_cursor->setVisible(true);
    reset();
  }
}

bool BlinkTimer::fire() {
  if (s_cursor && !s_cursor->bounds().isEmpty()) {
    s_cursor->switchVisible();
    return true;
  }
  return false;
}

}  // namespace Escher
