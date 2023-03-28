#include <math.h>
#include <omg/signaling_nan.h>
#include <quiz.h>

#include <cmath>

using namespace OMG;

QUIZ_CASE(omg_signaling_nan) {
  quiz_assert(OMG::IsSignalingNan(OMG::SignalingNan<float>()));
  quiz_assert(OMG::IsSignalingNan(OMG::SignalingNan<double>()));
  quiz_assert(!OMG::IsSignalingNan(static_cast<float>(NAN)));
  quiz_assert(!OMG::IsSignalingNan(static_cast<double>(NAN)));
#if !__EMSCRIPTEN__
  /* Emscripten cannot represent a NaN literal with custom bit pattern in
   * NAN-canonicalizing JS engines. We use a magic number instead, which is
   * not a NAN. */
  quiz_assert(std::isnan(OMG::SignalingNan<float>()));
  quiz_assert(std::isnan(OMG::SignalingNan<double>()));
#endif
}
