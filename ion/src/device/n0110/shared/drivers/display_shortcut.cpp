#include <shared/drivers/display.h>

namespace Ion {
namespace Display {

/* We shortcut the SVC made by POSTAndHardwareTests to direct calls to the
 * right drivers. */

bool waitForVBlank() {
  return Device::Display::waitForVBlank();
}

void POSTPushMulticolor(int rootNumberTiles, int tileSize) {
  Device::Display::POSTPushMulticolor(rootNumberTiles, tileSize);
}

void pullRect(KDRect r, KDColor * pixels) {
  Device::Display::pullRect(r, pixels);
}

void pushRect(KDRect r, const KDColor * pixels) {
  Device::Display::pushRect(r, pixels);
}

void pushRectUniform(KDRect r, KDColor color) {
  Device::Display::pushRectUniform(r, color);
}

}
}
