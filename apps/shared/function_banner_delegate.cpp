#include "function_banner_delegate.h"
#include <ion/unicode/utf8_decoder.h>
#include "poincare_helpers.h"
#include <poincare/preferences.h>

using namespace Poincare;

namespace Shared {

void FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, FunctionStore * functionStore, Poincare::Context * context) {
  ExpiringPointer<Function> function = functionStore->modelForRecord(record);
  constexpr int bufferSize = k_maxNumberOfCharacters+PrintFloat::charSizeForFloatsWithPrecision(Preferences::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = " ";
  int numberOfChar = 0;
  numberOfChar += UTF8Decoder::CodePointToChars(function->symbol(), buffer+numberOfChar, bufferSize-numberOfChar);
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer + numberOfChar, "=", bufferSize - numberOfChar);
  bannerView()->abscissaSymbol()->setText(buffer);

  constexpr int precision = Preferences::MediumNumberOfSignificantDigits;

  numberOfChar = PoincareHelpers::ConvertFloatToText<double>(cursor->t(), buffer, bufferSize, precision);
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer+numberOfChar, space, bufferSize - numberOfChar);
  bannerView()->abscissaValue()->setText(buffer);

  numberOfChar = function->nameWithArgument(buffer, bufferSize);
  assert(numberOfChar <= bufferSize);
  numberOfChar += strlcpy(buffer+numberOfChar, "=", bufferSize-numberOfChar);
  numberOfChar += function->printValue(cursor->t(), cursor->x(),cursor->y(), buffer+numberOfChar, bufferSize-numberOfChar, precision, context);
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->ordinateView()->setText(buffer);

  bannerView()->reload();
}

}
