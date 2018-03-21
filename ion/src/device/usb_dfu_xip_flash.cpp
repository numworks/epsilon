#include "usb/calculator.h"

namespace Ion {
namespace USB {

void DFU() {
  Ion::USB::Device::Calculator::Poll();
}

}
}
