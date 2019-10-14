#include "complex_model.h"

namespace Calculation {

ComplexModel::ComplexModel(std::complex<float> c) :
  Shared::CurveViewRange(),
  std::complex<float>(c)
{
}

float ComplexModel::xMin() const {
  return -1.2f*std::fabs(real());
}

float ComplexModel::xMax() const {
  return 1.2f*std::fabs(real());
}

float ComplexModel::yMin() const {
  return -1.2f*std::fabs(imag());
}

float ComplexModel::yMax() const {
  return 1.2f*std::fabs(imag());
}

}
