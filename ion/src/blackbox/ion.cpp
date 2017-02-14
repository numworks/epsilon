#include <stdint.h>
#include <ion.h>

void Ion::Display::pushRect(KDRect r, const KDColor * pixels) {
}

void Ion::Display::pushRectUniform(KDRect r, KDColor c) {
}

void Ion::Display::pullRect(KDRect r, KDColor * pixels) {
}

bool Ion::Keyboard::keyDown(Ion::Keyboard::Key key) {
  return false;
}

void Ion::msleep(long ms) {
}
