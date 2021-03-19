#include "calculator.h"
#include <userland/drivers/board.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  int currentKernelVersion = 12; //PlatformInfos.kernelVersion();
  int newKernelVersion = 12; //getPlatformInfo(userlandAddressOtherSlot()).kernelVersion --> implement on userland to know the header
  int currentUserlandVersion = 22;
  int newUserlandVersion = 24;
  Board::switchExecutableSlot(newKernelVersion - currentKernelVersion, newUserlandVersion - currentUserlandVersion); // FONCTION KERNEL QUI ADAPTE
}

}
}
}
