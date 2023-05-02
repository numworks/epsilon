#ifndef OMG_COMPARISON_H
#define OMG_COMPARISON_H

#include <cmath>

namespace OMG {

template <typename T>
T WithGreatestAbs(T x, T y) {
  return std::fabs(x) < std::fabs(y) ? y : x;
}

}  // namespace OMG

#endif
