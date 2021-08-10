#ifndef APPS_PROBABILITY_MODELS_CONSTANTS_H
#define APPS_PROBABILITY_MODELS_CONSTANTS_H

#include <poincare/print_float.h>

namespace Probability {

namespace Constants {

constexpr int k_shortBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(
    Poincare::Preferences::ShortNumberOfSignificantDigits);
constexpr int k_largeBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(
    Poincare::Preferences::LargeNumberOfSignificantDigits);

}  // namespace Constants

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_CONSTANTS_H */
