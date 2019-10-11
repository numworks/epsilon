#include "complex_model.h"

namespace Calculation {

ComplexModel::ComplexModel() :
  Shared::CurveViewRange(),
  m_complex(2.3f, 4.1f)
{
}

float ComplexModel::xMin() const {
  return -1.2f*std::fabs(m_complex.real());
}

float ComplexModel::xMax() const {
  return 1.2f*std::fabs(m_complex.real());
}

float ComplexModel::yMin() const {
  return -1.2f*std::fabs(m_complex.imag());
}

float ComplexModel::yMax() const {
  return 1.2f*std::fabs(m_complex.imag());
}

}
