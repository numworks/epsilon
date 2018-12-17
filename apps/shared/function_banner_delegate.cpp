#include "function_banner_delegate.h"
#include "poincare_helpers.h"
#include "../constant.h"

using namespace Poincare;

namespace Shared {

void FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Function * function, char symbol) {
  constexpr size_t bufferSize = k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = "                  ";
  const char * legend = "0=";
  int legendLength = strlen(legend);
  int numberOfChar = 0;
  strlcpy(buffer, legend, bufferSize);
  numberOfChar += legendLength;
  buffer[0] = symbol;
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(cursor->x(), buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize - numberOfChar);
  buffer[k_maxDigitLegendLength+2] = 0;
  bannerView()->setLegendAtIndex(buffer, 0);

  numberOfChar = 0;
  legend = "0(x)=";
  legendLength = strlen(legend);
  numberOfChar += legendLength;
  strlcpy(buffer, legend, bufferSize);
  buffer[2] = symbol;
  buffer[0] = function->name()[0];
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(cursor->y(), buffer+legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize - numberOfChar);
  buffer[k_maxDigitLegendLength+5] = 0;
  bannerView()->setLegendAtIndex(buffer, 1);
}

}
