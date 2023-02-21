#ifndef SHARED_PACKED_RANGE_1D_H
#define SHARED_PACKED_RANGE_1D_H

#include <float.h>
#include <poincare/range.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#include <stdint.h>
#endif

namespace Shared {

/* This class is used in a DataBuffer of a Storage::Record. See comment in
 * Shared::Function::RecordDataBuffer about packing. */

class __attribute__((packed)) PackedRange1D final {
 public:
  PackedRange1D(float min = -INFINITY, float max = INFINITY)
      : m_min(min), m_max(max) {}

  float min() const { return m_min; }
  float max() const { return m_max; }
  void setMin(float t, float limit = INFINITY) {
    privateSet(t, limit, &Poincare::Range1D::setMin);
  }
  void setMax(float t, float limit = INFINITY) {
    privateSet(t, limit, &Poincare::Range1D::setMax);
  }

 private:
  void privateSet(float t, float limit,
                  void (Poincare::Range1D::*setter)(float, float));

#if __EMSCRIPTEN__
  // See comment about emscripten alignment in
  // Shared::Function::RecordDataBuffer
  static_assert(sizeof(emscripten_align1_float) == sizeof(float),
                "emscripten_align1_float should have the same size as float");
  emscripten_align1_float m_min;
  emscripten_align1_float m_max;
#else
  float m_min;
  float m_max;
#endif
};

static_assert(
    Poincare::Range1D::k_minLength >= FLT_EPSILON,
    "InteractiveCurveViewRange's minimal float range is lower than float "
    "precision, it might draw uglily curves such as cos(x)^2+sin(x)^2");

}  // namespace Shared

#endif
