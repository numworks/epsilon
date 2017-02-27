#include <ion.h>
#include "console.h"

/* This file implements a serial console.
 * We use a 9600 8N1 serial port */

namespace Ion {
namespace Console {

void writeLine(const char * line) {
  while (*line != NULL) {
    Device::sendChar(*line++);
  }
  Device::sendChar('\r');
  Device::sendChar('\n');
}

void readLine(char * line, int maxLineLength) {
  if (maxLineLength <= 0) {
    return;
  }
  char * cursor = line;
  char * last = line+maxLineLength-1;
  while (true) {
    *cursor = Device::recvChar();
    if (*cursor == '\r' || cursor == last) {
      *cursor = 0;
      return;
    }
    cursor++;
  }
}
/*

  lastChar

  char c = Device::recvChar();

  char * lineEnd = line + maxLineLength;

  while (line < lineEnd) {
    *line++ = Device::recvChar();
    if (*line == '\r') {
      break;
    }
  }
  *line = 0;
}
*/

}
}

namespace Ion {
namespace Console {
namespace Device {

void init() {
  RCC.APB2ENR()->setUSART1EN(true);

  GPIOB.MODER()->setMode(3, GPIO::MODER::Mode::AlternateFunction);
  GPIOB.MODER()->setMode(6, GPIO::MODER::Mode::AlternateFunction);

  GPIOB.AFR()->setAlternateFunction(3, GPIO::AFR::AlternateFunction::AF7);
  GPIOB.AFR()->setAlternateFunction(6, GPIO::AFR::AlternateFunction::AF7);

  UARTPort.CR1()->setUE(true);
  UARTPort.CR1()->setTE(true);
  UARTPort.CR1()->setRE(true);

  // Set the Baud rate
  // base clock = 16 MHz
  // Baud rate = fAPB2/(16*USARTDIV)
  // USARTDIV = 104.16667
  //
  // DIV_Fraction = 16*0.16666667
  //  = 2.666667 -> 3
  // DIV_Mantissa = 104 = 0x68
  // USART_BRR = 0x683
  UARTPort.BRR()->setDIV_FRAC(3);
  UARTPort.BRR()->setDIV_MANTISSA(104);
}

char recvChar() {
  while (UARTPort.SR()->getRXNE() == 0) {
  }
  return (char)UARTPort.DR()->get();
}

void sendChar(char c) {
  while (UARTPort.SR()->getTXE() == 0) {
  }
  UARTPort.DR()->set(c);
}

}
}
}
