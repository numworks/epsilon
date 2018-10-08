#include "storage_cartesian_function.h"
#include "../shared/storage_expression_model_store.h"
#include "../shared/poincare_helpers.h"
#include <float.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Graph {

void StorageCartesianFunction::DefaultName(char buffer[], size_t bufferSize) {
  /* a default name is "f[number].func", for instance "f12.func", that does not
   * exist yet in the storage */
  size_t constantNameSize = 1 + 1 + strlen(GlobalContext::funcExtension) + 1; // 'f', '.', extension, null-terminating char
  assert(bufferSize > constantNameSize);
  // Write the f
  buffer[0] = 'f';
  // Find the next available number
  int currentNumber = 0;
  int dotCharIndex = -1;
  while (currentNumber < bufferSize - constantNameSize) {
    dotCharIndex = 1 + Poincare::Integer(currentNumber).serialize(&buffer[1], bufferSize - constantNameSize + 1);
    if (GlobalContext::RecordWithName(buffer).isNull()) {
      // Name found
      break;
    }
    currentNumber++;
  }
  // Write the extension
  assert(dotCharIndex > 1);
  buffer[dotCharIndex] = Ion::Storage::k_dotChar;
  strlcpy(&buffer[dotCharIndex+1], GlobalContext::funcExtension, bufferSize - (dotCharIndex+1));
}

StorageCartesianFunction StorageCartesianFunction::NewModel(Ion::Storage::Record record) {
  StorageCartesianFunction newFunction = StorageCartesianFunction(record);
  newFunction.setColor(KDColorRed);
  newFunction.setActive(true);
  newFunction.setDisplayDerivative(true);
  return newFunction;
}

bool StorageCartesianFunction::displayDerivative() const {
  return functionNode()->displayDerivative();
}

void StorageCartesianFunction::setDisplayDerivative(bool display) {
  functionNode()->setDisplayDerivative(display);
}

double StorageCartesianFunction::approximateDerivative(double x, Poincare::Context * context) const {
  const char xUnknown[] = {Symbol::SpecialSymbols::UnknownX, 0};
  Poincare::Derivative derivative(reducedExpression(context), Symbol(xUnknown, 1), Poincare::Float<double>(x)); // derivative takes ownership of Poincare::Float<double>(x) and the clone of expression
  /* TODO: when we approximate derivative, we might want to simplify the
   * derivative here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(derivative, *context);
}

double StorageCartesianFunction::sumBetweenBounds(double start, double end, Poincare::Context * context) const {
  Poincare::Integral integral(reducedExpression(context), Poincare::Float<double>(start), Poincare::Float<double>(end)); // Integral takes ownership of args
  /* TODO: when we approximate integral, we might want to simplify the integral
   * here. However, we might want to do it once for all x (to avoid lagging in
   * the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(integral, *context);
}

Expression::Coordinate2D StorageCartesianFunction::nextMinimumFrom(double start, double step, double max, Context * context) const {
  return reducedExpression(context).nextMinimum(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

Expression::Coordinate2D StorageCartesianFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  return reducedExpression(context).nextMaximum(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

double StorageCartesianFunction::nextRootFrom(double start, double step, double max, Context * context) const {
  return reducedExpression(context).nextRoot(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

Expression::Coordinate2D StorageCartesianFunction::nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::StorageFunction * function) const {
  Expression reducedExp = reducedExpression(context);
  return reducedExp.nextIntersection(symbol(), start, step, max, *context, Preferences::sharedPreferences()->angleUnit(), reducedExp);
}
void StorageCartesianFunction::setContent(const char * c) {
  Expression expressionToStore = StorageExpressionModel::contentFromString(c);
// TODO
  StorageExpressionModel.setContentExpression(expressionToStore);
}

}
