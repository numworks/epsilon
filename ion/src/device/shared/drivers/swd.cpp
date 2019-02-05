#include "swd.h"
#include <drivers/config/swd.h>

namespace Ion {
namespace Device {
namespace SWD {

void init() {
  for(const GPIOPin & g : Config::Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF0);
  }
}

void shutdown() {
  for(const GPIOPin & g : Config::Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }
}

}
}
}
