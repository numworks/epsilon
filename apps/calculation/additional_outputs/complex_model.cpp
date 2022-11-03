#include "complex_model.h"
#include <poincare/range.h>

using namespace Poincare;

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
  Range1D range1D = Range1D::RangeBetween(minFactor * value, maxFactor * value);
  return direction >= 0.0f ? range1D.max() : range1D.min();
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
