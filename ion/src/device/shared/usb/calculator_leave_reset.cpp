#include "calculator.h"
#include <drivers/reset.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  Reset::coreWhilePlugged();
}

}
}
}
