#include "storage_cartesian_function.h"
#include "../shared/storage_expression_model_store.h"
#include "../shared/poincare_helpers.h"
#include <float.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Graph {

StorageCartesianFunction::StorageCartesianFunction(const char * text, KDColor color) :
  Shared::StorageFunction(text, color),
  m_displayDerivative(false)
{
}

StorageCartesianFunction::StorageCartesianFunction(Ion::Storage::Record record) :
  Shared::StorageFunction(record, KDColorRed),
  m_displayDerivative(false)
{
  // TODO set attributes from record
}

double StorageCartesianFunction::approximateDerivative(double x, Poincare::Context * context) const {
  const char xUnknown[] = {Symbol::SpecialSymbols::UnknownX, 0};
  Poincare::Derivative derivative(expression(context).clone(), Symbol(xUnknown, 1), Poincare::Float<double>(x)); // derivative takes ownership of Poincare::Float<double>(x) and the clone of expression
  /* TODO: when we approximate derivative, we might want to simplify the
   * derivative here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(derivative, *context);
}

double StorageCartesianFunction::sumBetweenBounds(double start, double end, Poincare::Context * context) const {
  Poincare::Integral integral(expression(context).clone(), Poincare::Float<double>(start), Poincare::Float<double>(end)); // Integral takes ownership of args
  /* TODO: when we approximate integral, we might want to simplify the integral
   * here. However, we might want to do it once for all x (to avoid lagging in
   * the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(integral, *context);
}

Expression::Coordinate2D StorageCartesianFunction::nextMinimumFrom(double start, double step, double max, Context * context) const {
  return expression(context).nextMinimum(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

Expression::Coordinate2D StorageCartesianFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  return expression(context).nextMaximum(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

double StorageCartesianFunction::nextRootFrom(double start, double step, double max, Context * context) const {
  return expression(context).nextRoot(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

Expression::Coordinate2D StorageCartesianFunction::nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::StorageFunction * function) const {
  return expression(context).nextIntersection(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit(), function->expression(context));
}


}
