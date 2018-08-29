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
  constexpr int bufferSize = Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(precision);
  char responseBuffer[bufferSize+4] = {'A', 'D', 'C', '='}; // ADC=
  Poincare::PrintFloat::convertFloatToText<float>(result, responseBuffer+4, bufferSize, precision);
  reply(responseBuffer);
}

}
}
}
}
