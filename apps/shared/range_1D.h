#ifndef SHARED_RANGE_1D_H
#define SHARED_RANGE_1D_H

#include <cmath>
#include <float.h>

namespace Shared {

class Range1D {
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
  float m_min;
  float m_max;
};

static_assert(Range1D::k_minFloat >= FLT_EPSILON, "InteractiveCurveViewRange's minimal float range is lower than float precision, it might draw uglily curves such as cos(x)^2+sin(x)^2");

}

#endif
