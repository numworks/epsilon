#include <ion/console.h>
#include <kandinsky.h>
#include <ion/display.h>

namespace Ion {
namespace Console {

char readChar() {
  return '\0';
}

static KDPoint cursor = KDPointZero;

void writeChar(char c) {
  char text[2] = {c, 0};
  KDContext * ctx = KDIonContext::sharedContext();
  cursor = ctx->drawString(text, cursor);
  if (cursor.y() > Ion::Display::Height) {
    cursor = KDPoint(cursor.x(), 0);
  }
}

bool transmissionDone() {
  // Always true because we flush after each writeChar
  return true;
}

}
}
