#include <kernel/drivers/led.h>
#include <kernel/drivers/authentication.h>
#include <drivers/config/led.h>
#include <drivers/config/clocks.h>

namespace Ion {
namespace Device {
namespace LED {

using namespace Regs;

extern KDColor sLedColor;

void setColorSecure(KDColor c) {
  if (Authentication::clearanceLevel() != Ion::Authentication::ClearanceLevel::NumWorks) {
    sLedColor = KDColorBlack;
    return;
  }
  setColor(c);
}

}
}
}
