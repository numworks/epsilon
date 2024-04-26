#ifndef OMG_ROUND_H
#define OMG_ROUND_H

#include <limits.h>
#include <poincare/old/float.h>

namespace OMG {

template <typename T>
static inline T LaxToZero(T x) {
  return std::fabs(x) < Poincare::Float<T>::EpsilonLax() ? static_cast<T>(0.f)
                                                         : x;
}

constexpr int CeilDivision(int x, int y) { return (x + y - 1) / y; }

}  // namespace OMG

#endif
