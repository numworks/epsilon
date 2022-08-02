#include <ion/usb.h>
#include <shared/regs/otg.h>

using namespace Ion::Device::Regs;

namespace Ion {
namespace USB {

void clearEnumerationInterrupt() {
  OTG.GINTSTS()->setENUMDNE(true);
}

void enable() {
  // Get out of soft-disconnected state
  OTG.DCTL()->setSDIS(false);
}

void disable() {
  OTG.DCTL()->setSDIS(true);
}

}
}
