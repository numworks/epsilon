#include <ion/usb.h>
#include <drivers/usb.h>
#include <regs/regs.h>

namespace Ion {
namespace USB {

using namespace Ion::Device::Regs;

void clearEnumerationInterrupt() {
  OTG.GINTSTS()->setENUMDNE(true);
}

void enable() {
  // Get out of soft-disconnected state
  OTG.DCTL()->setSDIS(false);
}

void disable() {
  // Get into soft-disconnected state
  OTG.DCTL()->setSDIS(true);
}

}
}
