#include "function_banner_delegate.h"
#include "poincare_helpers.h"
#include <poincare/preferences.h>

using namespace Poincare;

namespace Shared {

void FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, FunctionStore * functionStore, char symbol) {
  ExpiringPointer<Function> function = functionStore->modelForRecord(record);
  constexpr int bufferSize = k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Preferences::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = " ";
  int numberOfChar = 0;
  buffer[numberOfChar++] = symbol;
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer + numberOfChar, "=", bufferSize - numberOfChar);
  bannerView()->abscissaSymbol()->setText(buffer);

  constexpr int precision = Preferences::MediumNumberOfSignificantDigits;

  numberOfChar = PoincareHelpers::ConvertFloatToText<double>(cursor->x(), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(precision), precision);
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer+numberOfChar, space, bufferSize - numberOfChar);
  bannerView()->abscissaValue()->setText(buffer);

  numberOfChar = function->nameWithArgument(buffer, bufferSize, symbol);
  assert(numberOfChar <= bufferSize);
  numberOfChar += strlcpy(buffer+numberOfChar, "=", bufferSize-numberOfChar);
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(cursor->y(), buffer+numberOfChar, bufferSize-numberOfChar, precision);
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->ordinateView()->setText(buffer);

  bannerView()->reload();
}

}
