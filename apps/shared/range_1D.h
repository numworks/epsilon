#ifndef SHARED_RANGE_1D_H
#define SHARED_RANGE_1D_H

#include <cmath>
#include <float.h>

namespace Shared {

/* This class is used in a DataBuffer of a Storage::Record. See comment in
 * Shared::Function::FunctionRecordDataBuffer about packing. */

#pragma pack(push,1)
class  Range1D final {
public:
  constexpr static float k_minFloat = 1E-4f;
  Range1D(float min = -10.0f, float max = 10.0f) :
    m_min(min),
    m_max(max)
  {}
  float min() const { return m_min; }
  float max() const { return m_max; }
  void setMin(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY);
  void setMax(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY);
  static float clipped(float x, bool isMax, float lowerMaxFloat, float upperMaxFloat);
  static float defaultRangeLengthFor(float position);
private:
#if __EMSCRIPTEN__
    // See comment about emscripten alignement in Shared::Function::FunctionRecordDataBuffer
    static_assert(sizeof(emscripten_align1_short) == sizeof(uint16_t), "emscripten_align1_short should have the same size as uint16_t");
    emscripten_align1_float m_min;
    emscripten_align1_float m_max;
#else
  float m_min;
  float m_max;
#endif
};
#pragma pack(pop)

static_assert(Range1D::k_minFloat >= FLT_EPSILON, "InteractiveCurveViewRange's minimal float range is lower than float precision, it might draw uglily curves such as cos(x)^2+sin(x)^2");

}

#endif
