#ifndef OMG_SIGNALING_NAN_H
#define OMG_SIGNALING_NAN_H

#include <stdint.h>

namespace OMG {

/* Signaling NAN indicating an unusual NAN value, for example a default cache
 * value, an uninitialized value. It is used to make the difference with quiet
 * NAN, which is the result of an operation, for example the value of a
 * function. */

union IntFloat {
  uint32_t i;
  float f;
};

union IntDouble {
  uint64_t i;
  double d;
};

#if __EMSCRIPTEN__
/* Emscripten cannot represent a NaN literal with custom bit pattern in
 * NaN-canonicalizing JS engines. We use a magic number instead. */
constexpr static IntFloat k_sNanFloat = IntFloat{.f = -1.7014118346e+38};
constexpr static IntDouble k_sNanDouble = IntDouble{.d = -1.7014118346e+38};
#else
/* 0x7fa00000 corresponds to std::numeric_limits<float>::signaling_NaN()
 * 0x7ff4000000000000 corresponds to
 * std::numeric_limits<double>::signaling_NaN() */
constexpr static IntFloat k_sNanFloat = IntFloat{.i = 0x7fa00000};
constexpr static IntDouble k_sNanDouble = IntDouble{.i = 0x7ff4000000000000};
#endif

static inline bool IsSignalingNan(float f) {
  return IntFloat{.f = f}.i == k_sNanFloat.i;
}
static inline bool IsSignalingNan(double d) {
  return IntDouble{.d = d}.i == k_sNanDouble.i;
}
template <typename T>
inline T SignalingNan();
template <>
inline float SignalingNan<float>() {
  return k_sNanFloat.f;
}
template <>
inline double SignalingNan<double>() {
  return k_sNanDouble.d;
}

}  // namespace OMG

#endif
