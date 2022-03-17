#include "usb.h"
#include <ion/usb.h>
#include <drivers/config/usb.h>

namespace Ion {
namespace Device {
namespace USB {

const char* stringDescriptor() {
  return Config::InterfaceStringDescriptor;
}

}
}
}
