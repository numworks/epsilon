#include <stdint.h>
#include <ion.h>

void Ion::Display::pushRect(KDRect r, const KDColor * pixels) {
}

void Ion::Display::pushRectUniform(KDRect r, KDColor c) {
}

void Ion::Display::pullRect(KDRect r, KDColor * pixels) {
}

bool Ion::USB::isPlugged() {
  return false;
}

void Ion::msleep(long ms) {
}

const char * Ion::serialNumber() {
  return "123";
}

void Ion::LED::setColor(KDColor) {
}

bool Ion::Battery::isCharging() {
  return false;
}

Ion::Battery::Charge Ion::Battery::level() {
  return Charge::FULL;
}

float Ion::Battery::voltage() {
  return 0.0f;
}

void Ion::Display::waitForVBlank() {
}
