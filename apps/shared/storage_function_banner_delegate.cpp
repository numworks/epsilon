#include "storage_function_banner_delegate.h"
#include "poincare_helpers.h"
#include "../constant.h"

using namespace Poincare;

namespace Shared {

void StorageFunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, StorageFunction * function, char symbol) {
  constexpr int bufferSize = k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = "                  ";
  int spaceLength = strlen(space);
  const char * legend = "0=";
  int legendLength = strlen(legend);
  int numberOfChar = 0;
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  buffer[0] = symbol;
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(cursor->x(), buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, spaceLength+1);
  buffer[k_maxDigitLegendLength+2] = 0;
  bannerView()->setLegendAtIndex(buffer, 0);

  numberOfChar = 0;
  numberOfChar += function->nameWithArgument(buffer, bufferSize, symbol);
  legend = "=";
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(cursor->y(), buffer+numberOfChar, bufferSize-numberOfChar, Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  buffer[k_maxDigitLegendLength+5] = 0;
  bannerView()->setLegendAtIndex(buffer, 1);
}

}
