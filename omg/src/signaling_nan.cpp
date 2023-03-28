#include <omg/signaling_nan.h>

namespace OMG {

template <>
float SignalingNan<float>() {
  return k_sNanFloat.f;
}
template <>
double SignalingNan<double>() {
  return k_sNanDouble.d;
}

}  // namespace OMG
