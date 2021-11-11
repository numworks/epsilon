#include <drivers/usb.h>
#include <drivers/board.h>
#include <drivers/config/usb.h>

namespace Ion {
namespace Device {

using namespace Regs;

namespace USB {

bool useAlternateFunctionVbus() {
  return Board::pcbVersion() == 0;
}

void initVbus() {
  if (useAlternateFunctionVbus()) {
    Config::VbusPin.init();
  } else {
    Config::VbusPin.group().MODER()->setMode(Config::VbusPin.pin(), GPIO::MODER::Mode::Input);
    Config::VbusPin.group().PUPDR()->setPull(Config::VbusPin.pin(), GPIO::PUPDR::Pull::None);
  }
}

}
}
}
