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

}  // namespace OMG
