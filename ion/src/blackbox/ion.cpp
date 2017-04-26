#include <stdint.h>
#include <ion.h>

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
