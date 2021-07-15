#include "command.h"
#include <drivers/battery.h>
#include <poincare/print_float.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void ADC(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  float result = Battery::voltage();
  constexpr int precision = 8;
  constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
  constexpr int glyphLength = Poincare::PrintFloat::glyphLengthForFloatWithPrecision(precision);
  char responseBuffer[bufferSize+4] = {'A', 'D', 'C', '='}; // ADC=
  Poincare::PrintFloat::ConvertFloatToText<float>(result, responseBuffer+4, bufferSize, glyphLength, precision, Poincare::Preferences::PrintFloatMode::Decimal);
  reply(responseBuffer);
}

}
}
}
}
