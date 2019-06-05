#include <usb/stack/device.h>
#include <drivers/reset.h>

namespace Ion {
namespace Device {
namespace USB {

void Device::leave() {
  Ion::Device::Reset::core();
}

}
}
}


