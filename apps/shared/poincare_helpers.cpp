#include "poincare_helpers.h"

namespace Shared {

namespace PoincareHelpers {

template <class T>
T ValueOfFloatAsDisplayed(T t, int precision, Poincare::Context * context) {
  assert(precision <= Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
  constexpr size_t bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
  char buffer[bufferSize];
  // Get displayed value
  size_t numberOfChar = ConvertFloatToText<T>(t, buffer, bufferSize, precision);
  assert(numberOfChar <= bufferSize);
  // Silence compiler warnings for assert
  (void) numberOfChar;
  // Extract displayed value
  return ApproximateToScalar<T>(buffer, context);
}

template float ValueOfFloatAsDisplayed<float>(float t, int precision, Poincare::Context * context);
template double ValueOfFloatAsDisplayed<double>(double t, int precision, Poincare::Context * context);
}

}