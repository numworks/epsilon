#include <usb/stack/device.h>
#include <drivers/reset.h>
#include <drivers/config/flash.h>

namespace Ion {
namespace Device {
namespace USB {

void Device::leave(uint32_t leaveAddress) {
  if (leaveAddress == Ion::Device::Flash::Config::StartAddress) {
    Ion::Device::Reset::core();
  } else {
    Ion::Device::Reset::jump(leaveAddress);
  }
}

}
}
}


