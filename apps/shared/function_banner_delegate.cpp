#include "function_banner_delegate.h"
#include "../constant.h"

using namespace Poincare;

namespace Shared {

void FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Function * function, char symbol) {
  char buffer[k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * space = "                  ";
  int spaceLength = strlen(space);
  const char * legend = "0=";
  int legendLength = strlen(legend);
  int numberOfChar = 0;
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  buffer[0] = symbol;
  numberOfChar += PrintFloat::convertFloatToText<double>(cursor->x(), buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, spaceLength+1);
  buffer[k_maxDigitLegendLength+2] = 0;
  bannerView()->setLegendAtIndex(buffer, 0);

  numberOfChar = 0;
  legend = "0(x)=";
  legendLength = strlen(legend);
  numberOfChar += legendLength;
  strlcpy(buffer, legend, legendLength+1);
  buffer[2] = symbol;
  buffer[0] = function->name()[0];
  numberOfChar += PrintFloat::convertFloatToText<double>(cursor->y(), buffer+legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, spaceLength+1);
  buffer[k_maxDigitLegendLength+5] = 0;
  bannerView()->setLegendAtIndex(buffer, 1);
}

}
