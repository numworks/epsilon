#include <usb/stack/device.h>
#include <drivers/reset.h>

namespace Ion {
namespace Device {
namespace USB {

void Device::leave(uint32_t leaveAddress) {
  /* We don't perform a core reset because at this point in time the USB cable
   * is most likely plugged in. Doing a full core reset would be the clean
   * thing to do but would therefore result in the device entering the ROMed
   * DFU bootloader, which we want to avoid. By performing a jump-reset, we
   * will enter the newly flashed firmware. */
  Ion::Device::Reset::jump(leaveAddress);
}

}
}
}

