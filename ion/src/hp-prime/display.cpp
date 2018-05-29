#include <ion.h>
#include "display.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}

// Public Ion::Display methods

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor * pixels) {
}

void pushRectUniform(KDRect r, KDColor c) {
}

void pullRect(KDRect r, KDColor * pixels) {
}

void waitForVBlank() {
}

}
}

// Private Ion::Display::Device methods

namespace Ion {
namespace Display {
namespace Device {

}
}
}
