#include <ion.h>
#include "console.h"

/* This file implements a serial console.
 * We use a 115200 8N1 serial port */

namespace Ion {
namespace Console {

char readChar() {
  while (Device::UARTPort.SR()->getRXNE() == 0) {
  }
  return (char)Device::UARTPort.DR()->get();
}

void writeChar(char c) {
  while (Device::UARTPort.SR()->getTXE() == 0) {
  }
  Device::UARTPort.DR()->set(c);
}

}
}

namespace Ion {
namespace Console {
namespace Device {

void init() {
  RCC.APB1ENR()->setUSART3EN(true);

  for(const GPIOPin & g : Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF7);
  }

  UARTPort.CR1()->setUE(true);
  UARTPort.CR1()->setTE(true);
  UARTPort.CR1()->setRE(true);

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
  UARTPort.BRR()->setDIV_MANTISSA(26);
  UARTPort.BRR()->setDIV_FRAC(1);
}

void shutdown() {
  for(const GPIOPin & g : Pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }
}

bool peerConnected() {
  RxPin.group().PUPDR()->setPull(RxPin.pin(), GPIO::PUPDR::Pull::Down);
  RxPin.group().MODER()->setMode(RxPin.pin(), GPIO::MODER::Mode::Input);
  msleep(1);
  bool result = RxPin.group().IDR()->get(RxPin.pin());
  RxPin.group().PUPDR()->setPull(RxPin.pin(), GPIO::PUPDR::Pull::None);
  RxPin.group().MODER()->setMode(RxPin.pin(), GPIO::MODER::Mode::AlternateFunction);
  return result;
}


}
}
}
