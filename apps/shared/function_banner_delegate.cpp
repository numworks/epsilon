#include "function_banner_delegate.h"
#include "poincare_helpers.h"
#include "../constant.h"

using namespace Poincare;

namespace Shared {

void FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, FunctionStore * functionStore, char symbol) {
  ExpiringPointer<Function> function = functionStore->modelForRecord(record);
  constexpr int bufferSize = k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = " ";
  int numberOfChar = 0;
  buffer[numberOfChar++] = symbol;
  strlcpy(buffer + numberOfChar, "=", bufferSize - numberOfChar);
  bannerView()->abscissaSymbol()->setText(buffer);

  numberOfChar = PoincareHelpers::ConvertFloatToText<double>(cursor->x(), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize - numberOfChar);
  bannerView()->abscissaValue()->setText(buffer);

  numberOfChar = function->nameWithArgument(buffer, bufferSize, symbol);
  numberOfChar += strlcpy(buffer+numberOfChar, "=", bufferSize-numberOfChar);
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(cursor->y(), buffer+numberOfChar, bufferSize-numberOfChar, Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->ordinateView()->setText(buffer);

  bannerView()->reload();
}

}
