#include "command.h"
#include <ion.h>
#include <poincare.h>
#include <ion/src/device/led.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void ADC(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  float result = Ion::Battery::voltage();
  constexpr int precision = 8;
  constexpr int bufferSize = Poincare::Complex::bufferSizeForFloatsWithPrecision(precision);
  char responseBuffer[bufferSize+4] = {'A', 'D', 'C', '='}; // ADC=
  Poincare::Complex::convertFloatToText(result, responseBuffer+4, bufferSize, precision);
  reply(responseBuffer);
}

}
}
}
}
