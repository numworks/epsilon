#include "complex_model.h"

namespace Calculation {

ComplexModel::ComplexModel(std::complex<float> c) :
  Shared::CurveViewRange(),
  std::complex<float>(c)
{
}

float rangeBound(float value, float direction) {
  if (std::isnan(value) || std::isinf(value) || value == 0.0f) {
    return direction;
  }
  float factor = direction*value >= 0.0f ? 1.2f : -0.3f;
  return factor*value;
}

float ComplexModel::xMin() const {
  return rangeBound(real(), -1.0f);
}

float ComplexModel::xMax() const {
  return rangeBound(real(), 1.0f);
}

float ComplexModel::yMin() const {
  return rangeBound(imag(), -1.0f);
}

float ComplexModel::yMax() const {
  return rangeBound(imag(), 1.0f);
}

}
