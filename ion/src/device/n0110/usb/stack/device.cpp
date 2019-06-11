#include <usb/stack/device.h>
#include <drivers/reset.h>

extern void * _jump_reset_address;

namespace Ion {
namespace Device {
namespace USB {

void Device::leave(uint32_t leaveAddress) {
  if (((void *)leaveAddress) == _core_reset_address) {
    Ion::Device::Reset::core();
  } else {
    Ion::Device::Reset::jump(leaveAddress);
  }
}

}
}
}


