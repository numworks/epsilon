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
  while (Config::UARTPort.SR()->getRXNE() == 0) {
  }
  return (char)Config::UARTPort.DR()->get();
}

void writeChar(char c) {
  while (Config::UARTPort.SR()->getTXE() == 0) {
  }
  Config::UARTPort.DR()->set(c);
}

}
}

namespace Ion {
namespace Device {
namespace Console {

void init() {
  RCC.APB1ENR()->setUSART3EN(true);

  for(const GPIOPin & g : Config::Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF7);
  }

  Config::UARTPort.CR1()->setUE(true);
  Config::UARTPort.CR1()->setTE(true);
  Config::UARTPort.CR1()->setRE(true);

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
  Config::UARTPort.BRR()->setDIV_MANTISSA(26);
  Config::UARTPort.BRR()->setDIV_FRAC(1);
}

void shutdown() {
  for(const GPIOPin & g : Config::Pins) {
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
