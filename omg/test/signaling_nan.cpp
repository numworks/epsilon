#include <math.h>
#include <omg/signaling_nan.h>
#include <quiz.h>

using namespace OMG;

QUIZ_CASE(omg_signaling_nan) {
  quiz_assert(OMG::IsSignalingNan(OMG::SignalingNan()));
  quiz_assert(!OMG::IsSignalingNan(static_cast<float>(NAN)));
}
