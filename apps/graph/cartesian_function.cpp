#include "cartesian_function.h"
#include <float.h>
#include <cmath>

using namespace Poincare;

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
  Poincare::Expression * args[2] = {expression(context), &abscissa};
  Poincare::Derivative derivative(args, true);
  /* TODO: when we will simplify derivative, we might want to simplify the
   * derivative here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return derivative.approximateToScalar<double>(*context);
}

double CartesianFunction::sumBetweenBounds(double start, double end, Poincare::Context * context) const {
  Poincare::Complex<double> x = Poincare::Complex<double>::Float(start);
  Poincare::Complex<double> y = Poincare::Complex<double>::Float(end);
  Poincare::Expression * args[3] = {expression(context), &x, &y};
  Poincare::Integral integral(args, true);
  /* TODO: when we will simplify integral, we might want to simplify the
   * integral here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return integral.approximateToScalar<double>(*context);
}

Expression::Coordinate2D CartesianFunction::nextMinimumFrom(double start, double step, double max, Context * context) const {
  return expression(context)->nextMinimum(symbol(), start, step, max, *context);
}

Expression::Coordinate2D CartesianFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  return expression(context)->nextMaximum(symbol(), start, step, max, *context);
}

double CartesianFunction::nextRootFrom(double start, double step, double max, Context * context) const {
  return expression(context)->nextRoot(symbol(), start, step, max, *context);
}

Expression::Coordinate2D CartesianFunction::nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::Function * function) const {
  return expression(context)->nextIntersection(symbol(), start, step, max, *context, function->expression(context));
}

char CartesianFunction::symbol() const {
  return 'x';
}

}
