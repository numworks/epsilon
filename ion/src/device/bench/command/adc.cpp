#include "command.h"
#include <ion.h>
#include <poincare/print_float.h>
#include <ion/src/device/shared/drivers/led.h>

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
  Poincare::PrintFloat::ConvertFloatToText<float>(result, responseBuffer+4, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
  reply(responseBuffer);
}

}
}
}
}
