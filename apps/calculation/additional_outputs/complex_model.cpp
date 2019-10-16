#include "complex_model.h"

namespace Calculation {

ComplexModel::ComplexModel(std::complex<float> c) :
  Shared::CurveViewRange(),
  std::complex<float>(c)
{
}

float rangeBound(float value, bool max) {
  if (std::isnan(value) || std::isinf(value)) {
    return max ? 1.0f : -1.0f;
  }
  float factor = max ? 1.2f : -1.2f;
  return factor*std::fabs(value);
}

float ComplexModel::xMin() const {
  return rangeBound(real(), false);
}

float ComplexModel::xMax() const {
  return rangeBound(real(), true);
}

float ComplexModel::yMin() const {
  return rangeBound(imag(), false);
}

float ComplexModel::yMax() const {
  return rangeBound(imag(), true);
}

}
