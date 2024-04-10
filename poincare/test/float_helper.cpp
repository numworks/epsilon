#include "float_helper.h"

#include <assert.h>
#include <quiz.h>

#include <algorithm>

template <typename T>
bool relatively_equal(T observed, T expected, T relativeThreshold) {
  assert(std::isfinite(observed) && std::isfinite(expected));
  if (expected == 0.0) {
    return observed == 0.0;
  }
  return std::fabs((observed - expected) / expected) <= relativeThreshold;
}

template <typename T>
bool roughly_equal(T observed, T expected, T threshold, bool acceptNAN,
                   T nullExpectedThreshold) {
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
  return relatively_equal(observed, expected, threshold);
}

template <typename T>
void assert_roughly_equal(T observed, T expected, T threshold, bool acceptNAN,
                          T nullExpectedThreshold) {
  quiz_assert(roughly_equal<T>(observed, expected, threshold, acceptNAN,
                               nullExpectedThreshold));
}

template bool roughly_equal<float>(float, float, float, bool, float);
template bool roughly_equal<double>(double, double, double, bool, double);
template void assert_roughly_equal<float>(float, float, float, bool, float);
template void assert_roughly_equal<double>(double, double, double, bool,
                                           double);
