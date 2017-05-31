#include "cartesian_function.h"

namespace Graph {

CartesianFunction::CartesianFunction(const char * text, KDColor color) :
  Shared::Function(text, color),
  m_displayDerivative(false)
{
}

bool CartesianFunction::displayDerivative() {
  return m_displayDerivative;
}

void CartesianFunction::setDisplayDerivative(bool display) {
  m_displayDerivative = display;
}

float CartesianFunction::approximateDerivative(float x, Poincare::Context * context) const {
  Poincare::Complex abscissa = Poincare::Complex::Float(x);
  Poincare::Expression * args[2] = {expression(), &abscissa};
  Poincare::Derivative derivative;
  derivative.setArgument(args, 2, true);
  return derivative.approximate(*context);
}

char CartesianFunction::symbol() const {
  return 'x';
}

}
