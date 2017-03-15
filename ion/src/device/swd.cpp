#include "swd.h"
#include "regs/regs.h"

namespace Ion {
namespace SWD {
namespace Device {

void init() {
  for(const GPIOPin & g : Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF0);
  }
}

void shutdown() {
  for(const GPIOPin & g : Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }
}

}
}
}
