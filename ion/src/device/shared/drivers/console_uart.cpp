#include "console.h"
#include <ion/console.h>
#include <ion/timing.h>
#include <drivers/config/console.h>

/* This file implements a serial console.
 * We use a 115200 8N1 serial port */

namespace Ion {
namespace Console {

using namespace Device::Console;

char readChar() {
  while (Config::Port.SR()->getRXNE() == 0) {
  }
  return (char)Config::Port.RDR()->get();
}

void writeChar(char c) {
  Config::Port.TDR()->set(c);
  // Wait until the write is done
  while (Config::Port.SR()->getTXE() == 0) {
  }
}

bool transmissionDone() {
  return Config::Port.SR()->getTC() == 1;
}

}
}

namespace Ion {
namespace Device {
namespace Console {

using namespace Regs;

constexpr static GPIOPin Pins[] = { Config::RxPin, Config::TxPin };

void init() {
  for(const GPIOPin & g : Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), Config::AlternateFunction);
  }

  Config::Port.CR1()->setUE(true);
  Config::Port.CR1()->setTE(true);
  Config::Port.CR1()->setRE(true);
  Config::Port.BRR()->set(Config::USARTDIVValue);
}

void shutdown() {
  for(const GPIOPin & g : Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }
}

bool peerConnected() {
  Config::RxPin.group().PUPDR()->setPull(Config::RxPin.pin(), GPIO::PUPDR::Pull::Down);
  Config::RxPin.group().MODER()->setMode(Config::RxPin.pin(), GPIO::MODER::Mode::Input);
  Timing::msleep(1);
  bool result = Config::RxPin.group().IDR()->get(Config::RxPin.pin());
  Config::RxPin.group().PUPDR()->setPull(Config::RxPin.pin(), GPIO::PUPDR::Pull::None);
  Config::RxPin.group().MODER()->setMode(Config::RxPin.pin(), GPIO::MODER::Mode::AlternateFunction);
  return result;
}

}
}
}
