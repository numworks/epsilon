#include "console.h"
#include <ion/console.h>
#include <ion/timing.h>
#include <drivers/config/console.h>

/* This file implements a serial console.
 * We use a 115200 8N1 serial port */

namespace Ion {
namespace Console {

using namespace Ion::Device::Console;

char readChar() {
  while (Config::Port.SR()->getRXNE() == 0) {
  }
  return (char)Config::Port.DR()->get();
}

void writeChar(char c) {
  while (Config::Port.SR()->getTXE() == 0) {
  }
  Config::Port.DR()->set(c);
}

}
}

namespace Ion {
namespace Device {
namespace Console {

constexpr static GPIOPin Pins[] = { Config::RxPin, Config::TxPin };

void init() {
  for(const GPIOPin & g : Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), Config::AlternateFunction);
  }

  Config::Port.CR1()->setUE(true);
  Config::Port.CR1()->setTE(true);
  Config::Port.CR1()->setRE(true);

  /* We need to set the baud rate of the UART port.
   * This is set relative to the APB1 clock, which runs at 48 MHz.
   *
   * The baud rate is set by the following equation:
   * BaudRate = fAPB1/(16*USARTDIV), where USARTDIV is a divider.
   * In other words, USARDFIV = fAPB1/(16*BaudRate). All frequencies in Hz.
   *
   * In our case, fAPB1 = 48 MHz, so USARTDIV = 26.0416667
   * DIV_MANTISSA = 26
   * DIV_FRAC = 16*0.0416667 = 1
   */
  Config::Port.BRR()->setDIV_MANTISSA(26);
  Config::Port.BRR()->setDIV_FRAC(1);
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
