#include <regs/regs.h>

namespace Ion {
namespace USB {

bool isEnumerated() {
  /* Note: This implementation is not perfect. One would assume isEnumerated to
   * return true for as long as the device is enumerated. But the GINTSTS
   * register will be cleared in the poll() routine. */
  return Ion::Device::Regs::OTG.GINTSTS()->getENUMDNE();
}

}
}
