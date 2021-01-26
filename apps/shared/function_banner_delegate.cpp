#include "function_banner_delegate.h"
#include <ion/unicode/utf8_decoder.h>
#include "poincare_helpers.h"
#include <poincare/preferences.h>

using namespace Poincare;

namespace Shared {


int convertDoubleToText(double t, char * buffer, int bufferSize) {
  return PoincareHelpers::ConvertFloatToText<double>(t, buffer, bufferSize, Preferences::sharedPreferences()->numberOfSignificantDigits());
}

void FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, FunctionStore * functionStore, Poincare::Context * context) {
  ExpiringPointer<Function> function = functionStore->modelForRecord(record);
  char buffer[k_textBufferSize];
  const char * space = " ";
  int numberOfChar = 0;
  numberOfChar += UTF8Decoder::CodePointToChars(function->symbol(), buffer+numberOfChar, k_textBufferSize-numberOfChar);
  assert(numberOfChar <= k_textBufferSize);
  strlcpy(buffer + numberOfChar, "=", k_textBufferSize - numberOfChar);
  bannerView()->abscissaSymbol()->setText(buffer);

  numberOfChar = convertDoubleToText(cursor->t(), buffer, k_textBufferSize);
  assert(numberOfChar <= k_textBufferSize);
  strlcpy(buffer+numberOfChar, space, k_textBufferSize - numberOfChar);
  bannerView()->abscissaValue()->setText(buffer);

  numberOfChar = function->nameWithArgument(buffer, k_textBufferSize);
  assert(numberOfChar <= k_textBufferSize);
  numberOfChar += strlcpy(buffer+numberOfChar, "=", k_textBufferSize-numberOfChar);
  numberOfChar += function->printValue(cursor->t(), cursor->x(),cursor->y(), buffer+numberOfChar, k_textBufferSize-numberOfChar, Preferences::sharedPreferences()->numberOfSignificantDigits(), context);
  assert(numberOfChar <= k_textBufferSize);
  strlcpy(buffer+numberOfChar, space, k_textBufferSize-numberOfChar);
  bannerView()->ordinateView()->setText(buffer);

  bannerView()->reload();
}

double FunctionBannerDelegate::getValueDisplayedOnBanner(double t, Poincare::Context * context, double deltaThreshold, bool roundToZero) {
  if (roundToZero && std::fabs(t) < deltaThreshold) {
    // Round to 0 to avoid rounding to unnecessary low non-zero value.
    return 0.0;
  }
  // Convert float to text
  char buffer[k_textBufferSize];
  int numberOfChar = convertDoubleToText(t, buffer, k_textBufferSize);
  assert(numberOfChar <= k_textBufferSize);
  // Silence compiler warnings
  (void) numberOfChar;
  // Extract displayed value
  double displayedValue = PoincareHelpers::ApproximateToScalar<double>(buffer, context);
  // Return displayed value if difference from t is under deltaThreshold
  return std::fabs(displayedValue-t) < deltaThreshold ? displayedValue : t;
}

}
