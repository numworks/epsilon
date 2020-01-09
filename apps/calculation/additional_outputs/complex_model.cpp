#include "complex_model.h"

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
    return direction*maxFactor;
  }
  float factor = direction*value >= 0.0f ? maxFactor : minFactor;
  return factor*value;
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
