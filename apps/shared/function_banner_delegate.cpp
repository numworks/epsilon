#include "function_banner_delegate.h"
#include <ion/unicode/utf8_decoder.h>
#include "poincare_helpers.h"
#include <poincare/preferences.h>

using namespace Poincare;

namespace Shared {

constexpr int k_precision = Preferences::MediumNumberOfSignificantDigits;

int convertDoubleToText(double t, char * buffer, int bufferSize) {
  return PoincareHelpers::ConvertFloatToText<double>(t, buffer, bufferSize, k_precision);
}

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

  numberOfChar = convertDoubleToText(cursor->t(), buffer, bufferSize);
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer+numberOfChar, space, bufferSize - numberOfChar);
  bannerView()->abscissaValue()->setText(buffer);

  numberOfChar = function->nameWithArgument(buffer, bufferSize);
  assert(numberOfChar <= bufferSize);
  numberOfChar += strlcpy(buffer+numberOfChar, "=", bufferSize-numberOfChar);
  numberOfChar += function->printValue(cursor->t(), cursor->x(),cursor->y(), buffer+numberOfChar, bufferSize-numberOfChar, k_precision, context);
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->ordinateView()->setText(buffer);

  bannerView()->reload();
}

double FunctionBannerDelegate::getValueDisplayedOnBanner(double t, Poincare::Context * context, double deltaThreshold, bool roundToZero) {
  if (roundToZero && std::fabs(t) < deltaThreshold) {
    // Round to 0 to avoid rounding to unnecessary low non-zero value.
    return 0.0;
  }
  // Convert float to text
  constexpr int bufferSize = k_maxNumberOfCharacters+PrintFloat::charSizeForFloatsWithPrecision(k_precision);
  char buffer[bufferSize];
  int numberOfChar = convertDoubleToText(t, buffer, bufferSize);
  assert(numberOfChar <= bufferSize);
  // Silence compiler warnings
  (void) numberOfChar;
  // Extract displayed value
  double displayedValue = PoincareHelpers::ApproximateToScalar<double>(buffer, context);
  // Return displayed value if difference from t is under deltaThreshold
  return std::fabs(displayedValue-t) < deltaThreshold ? displayedValue : t;
}

}
