#ifndef OMG_ROUND_H
#define OMG_ROUND_H

#include <poincare/float.h>

namespace OMG {

template <typename T>
static inline T LaxToZero(T x) {
  return std::fabs(x) < Poincare::Float<T>::EpsilonLax() ? static_cast<T>(0.f)
                                                         : x;
}

// std::ceil is not constexpr
constexpr int Ceil(float f) {
  const int i = static_cast<int>(f);
  return f > static_cast<float>(i) ? i + 1 : i;
}

}  // namespace OMG

#endif
