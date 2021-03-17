#include "calculator.h"
#include <kernel/drivers/authentication.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  bool validKernelVersion = true; //PlatformInfos.kernelVersion() != getPlatformInfo(userlandAddressOtherSlot()).kernelVersion --> implement on userland to know the header
  Authentication::checkUserland(validKernelVersion);
}

}
}
}
