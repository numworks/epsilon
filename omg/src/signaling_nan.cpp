#include <omg/signaling_nan.h>

namespace OMG {

template <typename T>
T SignalingNan() {
  return sizeof(T) == sizeof(float) ? static_cast<T>(k_sNanFloat.f)
                                    : static_cast<T>(k_sNanDouble.d);
}

template float SignalingNan<float>();
template double SignalingNan<double>();

}  // namespace OMG
