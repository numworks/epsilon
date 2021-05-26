#include "function_banner_delegate.h"
#include <ion/unicode/utf8_decoder.h>
#include "poincare_helpers.h"
#include <poincare/preferences.h>

using namespace Poincare;

namespace Shared {

void FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, FunctionStore * functionStore, Poincare::Context * context) {
  ExpiringPointer<Function> function = functionStore->modelForRecord(record);
  char buffer[k_textBufferSize];
  int numberOfChar = 0;
  numberOfChar += UTF8Decoder::CodePointToChars(function->symbol(), buffer+numberOfChar, k_textBufferSize-numberOfChar);
  assert(numberOfChar <= k_textBufferSize);
  strlcpy(buffer + numberOfChar, "=", k_textBufferSize - numberOfChar);
  bannerView()->abscissaSymbol()->setText(buffer);

  numberOfChar = PoincareHelpers::ConvertFloatToText<double>(cursor->t(), buffer, k_textBufferSize, Preferences::sharedPreferences()->numberOfSignificantDigits());
  assert(numberOfChar < k_textBufferSize);
  buffer[numberOfChar++] = '\0';
  bannerView()->abscissaValue()->setText(buffer);

  numberOfChar = function->nameWithArgument(buffer, k_textBufferSize);
  assert(numberOfChar <= k_textBufferSize);
  numberOfChar += strlcpy(buffer+numberOfChar, "=", k_textBufferSize-numberOfChar);
  numberOfChar += function->printValue(cursor->t(), cursor->x(),cursor->y(), buffer+numberOfChar, k_textBufferSize-numberOfChar, Preferences::sharedPreferences()->numberOfSignificantDigits(), context);
  assert(numberOfChar < k_textBufferSize);
  buffer[numberOfChar++] = '\0';
  bannerView()->ordinateView()->setText(buffer);

  bannerView()->reload();
}

double FunctionBannerDelegate::getValueDisplayedOnBanner(double t, Poincare::Context * context, double deltaThreshold, bool roundToZero) {
  if (roundToZero && std::fabs(t) < deltaThreshold) {
    // Round to 0 to avoid rounding to unnecessary low non-zero value.
    return 0.0;
  }
  // Round to displayed value
  double displayedValue = PoincareHelpers::ValueOfFloatAsDisplayed<double>(t, Preferences::sharedPreferences()->numberOfSignificantDigits(), context);
  // Return displayed value if difference from t is under deltaThreshold
  return std::fabs(displayedValue-t) < deltaThreshold ? displayedValue : t;
}

}
