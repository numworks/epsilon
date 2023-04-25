#ifndef OMG_ROUND_H
#define OMG_ROUND_H

#include <limits.h>
#include <poincare/float.h>

namespace OMG {

template <typename T>
static inline T LaxToZero(T x) {
  return std::fabs(x) < Poincare::Float<T>::EpsilonLax() ? static_cast<T>(0.f)
                                                         : x;
}

// std::ceil is not constexpr
constexpr int Ceil(float f) {
  assert(static_cast<float>(INT_MIN) <= f && f <= static_cast<float>(INT_MAX));
  const int i = static_cast<int>(f);
  return f > static_cast<float>(i) ? i + 1 : i;
}

}  // namespace OMG

#endif
