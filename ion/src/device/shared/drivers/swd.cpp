#include "swd.h"
#include <drivers/config/swd.h>

namespace Ion {
namespace Device {
namespace SWD {

void init() {
  for(const AFGPIOPin & p : Config::Pins) {
    p.init();
  }
}

void shutdown() {
  for(const AFGPIOPin & p : Config::Pins) {
    p.shutdown();
  }
}

}
}
}
