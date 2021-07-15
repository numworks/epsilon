/* The bench uses methods from Kandinsky to draw text on the screen, which
 * call methods defined as svcalls in the userland. Since the bench does not
 * run with a kernel, we simply forward the low level methods here. */

#include <shared/drivers/display.h>

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor * pixels) {
  Device::Display::pushRect(r, pixels);
}

void pushRectUniform(KDRect r, KDColor c) {
  Device::Display::pushRectUniform(r, c);
}

void pullRect(KDRect r, KDColor * pixels) {
  Device::Display::pullRect(r, pixels);
}

}
}
