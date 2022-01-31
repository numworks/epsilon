#include "complex_model.h"
#include <apps/shared/range_1D.h>

namespace Calculation {

ComplexModel::ComplexModel(std::complex<float> c) :
  Shared::CurveViewRange(),
  std::complex<float>(c)
{
}

float ComplexModel::rangeBound(float direction, bool horizontal) const {
  float minFactor = k_minVerticalMarginFactor;
  float maxFactor = k_maxVerticalMarginFactor;
  float value = imag();
  if (horizontal) {
    minFactor = k_minHorizontalMarginFactor;
    maxFactor = k_maxHorizontalMarginFactor;
    value = real();
  }
  if (std::isnan(value) || std::isinf(value) || value == 0.0f) {
    return direction * maxFactor;
  }
  float r1 = minFactor * value;
  float r2 = maxFactor * value;
  r2 = Shared::Range1D::checkedValue(r2, nullptr, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, value >= 0.0f);
  r1 = Shared::Range1D::checkedValue(r1, &r2, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, value < 0.0f);
  return direction * value >= 0.0f ? r2 : r1;
}

float ComplexModel::xMin() const {
  return rangeBound(-1.0f, true);
}

float ComplexModel::xMax() const {
  return rangeBound(1.0f, true);
}

float ComplexModel::yMin() const {
  return rangeBound(-1.0f, false);
}

float ComplexModel::yMax() const {
  return rangeBound(1.0f, false);
}

}
