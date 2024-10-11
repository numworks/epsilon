#ifndef OMG_FLOAT_H
#define OMG_FLOAT_H

#include <assert.h>
#include <float.h>

#include <algorithm>
#include <cmath>

namespace OMG::Float {

bool IsGreater(float xI, float xJ, bool nanIsGreatest);

// Return true if observed and expected are approximately equal
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

// Helper for the compile-time square root
template <typename T>
consteval T SquareRootHelper(T x, T a, T b) {
  return a == b ? a : SquareRootHelper(x, static_cast<T>(0.5) * (a + x / a), a);
};

template <typename T>
consteval T SquareRoot(T x) {
  return x >= 0 ? SquareRootHelper(x, x, static_cast<T>(0.))
                : static_cast<T>(NAN);
}

template <typename T>
constexpr T EpsilonLax();

template <typename T>
constexpr T Epsilon();

template <typename T>
constexpr T SqrtEpsilonLax();

template <typename T>
constexpr T SqrtEpsilon();

template <typename T>
constexpr T Min();

template <typename T>
constexpr T Max();

/* To prevent incorrect approximations, such as cos(1.5707963267949) = 0
 * we made the neglect threshold stricter. This way, the approximation is more
 * selective.
 * However, when ploting functions such as e^(i.pi+x), the float approximation
 * fails by giving non-real results and therefore, the function appears "undef".
 * As a result we created two functions Epsilon that behave differently
 * according to the number's type. When it is a double we want maximal precision
 * -> precision_double = 1x10^(-15).
 * When it is a float, we accept more agressive approximations
 * -> precision_float = x10^(-6). */

template <>
constexpr inline float EpsilonLax<float>() {
  return 1E-6f;
}
template <>
constexpr inline double EpsilonLax<double>() {
  return 1E-15;
}
template <>
constexpr inline float Epsilon<float>() {
  return FLT_EPSILON;
}
template <>
constexpr inline double Epsilon<double>() {
  return DBL_EPSILON;
}
template <>
constexpr inline float SqrtEpsilonLax<float>() {
  return 1e-3f;
}
template <>
constexpr inline double SqrtEpsilonLax<double>() {
  return 3e-8f;
}
template <>
constexpr inline float SqrtEpsilon<float>() {
  return SquareRoot<float>(Epsilon<float>());
}
template <>
constexpr inline double SqrtEpsilon<double>() {
  return SquareRoot<double>(Epsilon<double>());
}
template <>
constexpr inline float Min<float>() {
  return FLT_MIN;
}
template <>
constexpr inline double Min<double>() {
  return DBL_MIN;
}
template <>
constexpr inline float Max<float>() {
  return FLT_MAX;
}
template <>
constexpr inline double Max<double>() {
  return DBL_MAX;
}

}  // namespace OMG::Float

#endif
