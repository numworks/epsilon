#include "complex_model.h"

namespace Calculation {

ComplexModel::ComplexModel(std::complex<float> c) :
  Shared::CurveViewRange(),
  std::complex<float>(c)
{
}

float rangeBound(float value, float direction, bool horizontal) {
  if (std::isnan(value) || std::isinf(value) || value == 0.0f) {
    return direction;
  }
  float minFactor = -0.5f;
  float maxFactor = 1.2f;
  if (horizontal) {
    minFactor = -1.0f;
    maxFactor = 2.0f;
  }
  float factor = direction*value >= 0.0f ? maxFactor : minFactor;
  return factor*value;
}

float ComplexModel::xMin() const {
  return rangeBound(real(), -1.0f, true);
}

float ComplexModel::xMax() const {
  return rangeBound(real(), 1.0f, true);
}

float ComplexModel::yMin() const {
  return rangeBound(imag(), -1.0f, false);
}

float ComplexModel::yMax() const {
  return rangeBound(imag(), 1.0f, false);
}

}
