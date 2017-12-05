#include "function_banner_delegate.h"
#include "../constant.h"

using namespace Poincare;

namespace Shared {

void FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Function * function, char symbol) {
  char buffer[k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char space[] = "                  ";
  int numberOfChar = 0;
  strcpy(buffer, "0=");
  numberOfChar += sizeof("0=") - 1;
  buffer[0] = symbol;
  numberOfChar += PrintFloat::convertFloatToText<double>(cursor->x(), buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strcpy(buffer+numberOfChar, space);
  buffer[k_maxDigitLegendLength+2] = 0;
  bannerView()->setLegendAtIndex(buffer, 0);

  numberOfChar = 0;
  numberOfChar += sizeof("0(x)=") - 1;
  strcpy(buffer, "0(x)=");
  buffer[2] = symbol;
  buffer[0] = function->name()[0];
  numberOfChar += PrintFloat::convertFloatToText<double>(cursor->y(), buffer+sizeof("0(x)=")-1, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strcpy(buffer+numberOfChar, space);
  buffer[k_maxDigitLegendLength+5] = 0;
  bannerView()->setLegendAtIndex(buffer, 1);
}

}
