#include "calculator.h"
#include <kernel/drivers/authentication.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  int currentKernelVersion = 12; //PlatformInfos.kernelVersion();
  int newKernelVersion = 12; //getPlatformInfo(userlandAddressOtherSlot()).kernelVersion --> implement on userland to know the header
  int currentUserlandVersion = 22;
  int newUserlandVersion = 24;
  Authentication::updateTrustAndSwitchSlot(currentKernelVersion, newKernelVersion, currentUserlandVersion, newUserlandVersion);
}

}
}
}
