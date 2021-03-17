#include "calculator.h"
#include <drivers/reset.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  if (leaveAddress == Ion::Device::InternalFlash::Config::StartAddress) {
    Reset::coreWhilePlugged();
  } else {
    Reset::jump(leaveAddress);
  }
}

}
}
}
