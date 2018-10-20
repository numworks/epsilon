#include "cartesian_function.h"
#include "../shared/poincare_helpers.h"
#include <float.h>
#include <cmath>

#include <poincare/derivative.h>
#include <poincare/integral.h>

using namespace Poincare;
using namespace Shared;

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
  Poincare::Derivative derivative(expression(context).clone(), Poincare::Float<double>(x)); // derivative takes ownership of Poincare::Float<double>(x) and the clone of expression
  /* TODO: when we will approximate derivative, we might want to simplify the
   * derivative here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(derivative, *context);
}

double CartesianFunction::sumBetweenBounds(double start, double end, Poincare::Context * context) const {
  Poincare::Integral integral(expression(context).clone(), Poincare::Float<double>(start), Poincare::Float<double>(end)); // Integral takes ownership of args
  /* TODO: when we will approximate integral, we might want to simplify the
   * integral here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(integral, *context);
}

Expression::Coordinate2D CartesianFunction::nextMinimumFrom(double start, double step, double max, Context * context) const {
  return expression(context).nextMinimum(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

Expression::Coordinate2D CartesianFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  return expression(context).nextMaximum(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

double CartesianFunction::nextRootFrom(double start, double step, double max, Context * context) const {
  return expression(context).nextRoot(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

Expression::Coordinate2D CartesianFunction::nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::Function * function) const {
  return expression(context).nextIntersection(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit(), function->expression(context));
}

char CartesianFunction::symbol() const {
  return 'x';
}

}
