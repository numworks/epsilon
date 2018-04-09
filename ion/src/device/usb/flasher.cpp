#include "../regs/regs.h"
#include "../usb/calculator.h"
#include <ion.h>

void ion_main(int argc, char * argv[]) {
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(0xFFFF00));
  while (true) {
    Ion::USB::enable();
    while (!OTG.GINTSTS()->getENUMDNE()) {
    }
    Ion::USB::Device::Calculator::PollAndReset(false);
  }
}
