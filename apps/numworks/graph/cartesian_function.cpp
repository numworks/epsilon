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

double CartesianFunction::approximateDerivative(double x, Poincare::Context * context) const {
  Poincare::Complex<double> abscissa = Poincare::Complex<double>::Float(x);
  Poincare::Expression * args[2] = {expression(), &abscissa};
  Poincare::Derivative derivative;
  derivative.setArgument(args, 2, true);
  return derivative.approximate<double>(*context);
}

char CartesianFunction::symbol() const {
  return 'x';
}

}
