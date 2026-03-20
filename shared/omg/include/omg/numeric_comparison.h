#pragma once

#include <cmath>

namespace OMG {

template <typename T>
T WithGreatestAbs(T x, T y) {
  return std::fabs(x) < std::fabs(y) ? y : x;
}

constexpr static inline bool EqualOrBothNan(double a, double b) {
  return a == b || (std::isnan(a) && std::isnan(b));
}

template <typename T>
constexpr bool AbsoluteValueComparison(T v1, T v2) {
  return std::fabs(v1) < std::fabs(v2);
}

}  // namespace OMG
