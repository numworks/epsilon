#include "storage_cartesian_function.h"
#include "storage_expression_model_store.h"
#include "poincare_helpers.h"
#include <float.h>
#include <cmath>

using namespace Poincare;

namespace Shared {

void StorageCartesianFunction::DefaultName(char buffer[], size_t bufferSize) {
  /* a default name is "f[number].func", for instance "f12.func", that does not
   * exist yet in the storage */
  size_t extensionLength = 1 + strlen(GlobalContext::funcExtension); // '.', extension, no null-terminating char
  size_t constantNameLength = 1 + extensionLength; // 'f', '.', extension, no null-terminating char
  assert(bufferSize > constantNameLength+1);
  // Write the f
  buffer[0] = 'f';
  // Find the next available number
  int currentNumber = 0;
  int currentNumberLength = -1;
  size_t availableBufferSize = bufferSize - constantNameLength;
  while (currentNumberLength < availableBufferSize) {
    currentNumberLength = Poincare::Integer(currentNumber).serialize(&buffer[1], availableBufferSize);
    if (GlobalContext::RecordBaseNameIsFree(buffer)) {
      // Name found
      break;
    }
    currentNumber++;
  }
  assert(currentNumberLength > 1 && currentNumberLength < availableBufferSize);
  // Write the extension
  int dotCharIndex = 1 + currentNumberLength;
  buffer[dotCharIndex] = Ion::Storage::k_dotChar;
  strlcpy(&buffer[dotCharIndex+1], GlobalContext::funcExtension, bufferSize - (dotCharIndex+1));
}

StorageCartesianFunction StorageCartesianFunction::NewModel(Ion::Storage::Record::ErrorStatus * error) {
  char nameBuffer[SymbolAbstract::k_maxNameSize];
  DefaultName(nameBuffer, SymbolAbstract::k_maxNameSize);
  CartesianFunctionRecordData data;
  *error = Ion::Storage::sharedStorage()->createRecordWithFullName(nameBuffer, &data, sizeof(data));
  if (*error != Ion::Storage::Record::ErrorStatus::None) {
    return StorageCartesianFunction();
  }
  return StorageCartesianFunction(Ion::Storage::sharedStorage()->recordNamed(nameBuffer));
}

int StorageCartesianFunction::derivativeNameWithArgument(char * buffer, size_t bufferSize, char arg) {
  // Fill buffer with f(x). Keep one char for derivative sign.
  int numberOfChars = nameWithArgument(buffer, bufferSize-1, arg);
  assert(numberOfChars < bufferSize - 1);
  char * lastChar = buffer+numberOfChars;
  while (*(lastChar+1) != '(' && lastChar >= buffer) {
    *(lastChar+1) = *lastChar;
    lastChar--;
  }
  *lastChar = '\'';
  return numberOfChars+1;
}

bool StorageCartesianFunction::displayDerivative() const {
  return recordData()->displayDerivative();
}

void StorageCartesianFunction::setDisplayDerivative(bool display) {
  return recordData()->setDisplayDerivative(display);
}

double StorageCartesianFunction::approximateDerivative(double x, Poincare::Context * context) const {
  Poincare::Derivative derivative(expression(context).clone(), Symbol(Symbol::SpecialSymbols::UnknownX), Poincare::Float<double>(x)); // derivative takes ownership of Poincare::Float<double>(x) and the clone of expression
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
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return expression(context).nextMinimum(unknownX, start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

Expression::Coordinate2D StorageCartesianFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return expression(context).nextMaximum(unknownX, start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

double StorageCartesianFunction::nextRootFrom(double start, double step, double max, Context * context) const {
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return expression(context).nextRoot(unknownX, start, step, max, *context, Preferences::sharedPreferences()->angleUnit());
}

Expression::Coordinate2D StorageCartesianFunction::nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::StorageFunction * function) const {
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return expression(context).nextIntersection(unknownX, start, step, max, *context, Preferences::sharedPreferences()->angleUnit(), function->expression(context));
}

StorageCartesianFunction::CartesianFunctionRecordData * StorageCartesianFunction::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<CartesianFunctionRecordData *>(const_cast<void *>(d.buffer));
}

}
