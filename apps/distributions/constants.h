#ifndef DISTRIBUTIONS_CONSTANTS_H
#define DISTRIBUTIONS_CONSTANTS_H

#include <poincare/print_float.h>

namespace Distributions {

namespace Constants {

constexpr int k_shortBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(
    Poincare::Preferences::ShortNumberOfSignificantDigits);
constexpr int k_largeBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(
    Poincare::Preferences::VeryLargeNumberOfSignificantDigits);

constexpr int k_shortFloatNumberOfChars = k_shortBufferSize - 1;
constexpr int k_largeFloatNumberOfChars = k_largeBufferSize - 1;

}  // namespace Constants

}  // namespace Distributions

#endif /* DISTRIBUTIONS_CONSTANTS_H */
