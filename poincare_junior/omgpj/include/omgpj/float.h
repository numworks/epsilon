#ifndef UTILS_FLOAT_H
#define UTILS_FLOAT_H

#include <assert.h>

#include <algorithm>
#include <cmath>

namespace OMG::Float {

bool IsGreater(float xI, float xJ, bool nanIsGreatest);

// TODO: This is a copy of Poincare::Helpers::RelativelyEqual
template <typename T>
bool RelativelyEqual(T observed, T expected, T relativeThreshold) {
  assert(std::isfinite(observed) && std::isfinite(expected));
  return (expected == 0.0)
             ? (observed == 0.0)
             : std::fabs((observed - expected) / expected) <= relativeThreshold;
}

// TODO: This is a copy of Poincare::Test::Helper::roughly_equal
/* Return true if observed and expected are approximately equal, according to
 * threshold and acceptNAN parameters. */
template <typename T>
bool RoughlyEqual(T observed, T expected, T threshold, bool acceptNAN = false,
                  T nullExpectedThreshold = NAN) {
  if (std::isnan(observed) || std::isnan(expected)) {
    return acceptNAN && std::isnan(observed) && std::isnan(expected);
  }
  T max = std::max(std::fabs(observed), std::fabs(expected));
  if (max == INFINITY) {
    return observed == expected;
  }
  if (expected == 0.0) {
    if (std::isnan(nullExpectedThreshold)) {
      nullExpectedThreshold = threshold;
    }
    return max <= nullExpectedThreshold;
  }
  return RelativelyEqual<T>(observed, expected, threshold);
}

}  // namespace OMG::Float

#endif
