#include "dfu_privileged.h"
#include "calculator.h"

namespace Ion {
namespace USB {

void DFU() {
  Device::USB::DFUHandler();
}

}
}


namespace Ion {
namespace Device {
namespace USB {

void DFUHandler() {
  Calculator::PollAndReset(true);
}

}
}
}
