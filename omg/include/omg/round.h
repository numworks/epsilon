#ifndef OMG_ROUND_H
#define OMG_ROUND_H

#include <poincare/float.h>

namespace OMG {

template <typename T>
static inline T LaxToZero(T x) {
  return std::fabs(x) < Poincare::Float<T>::EpsilonLax() ? static_cast<T>(0.f)
                                                         : x;
}

}  // namespace OMG

#endif
