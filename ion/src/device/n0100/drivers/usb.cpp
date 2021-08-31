#include <drivers/usb.h>
#include <drivers/config/usb.h>

namespace Ion {
namespace Device {
namespace USB {

void initVbus() {
  Config::VbusPin.init();
}

}
}
}
