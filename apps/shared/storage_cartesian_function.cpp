#include "storage_cartesian_function.h"
#include "storage_expression_model_store.h"
#include "poincare_helpers.h"
#include <poincare/derivative.h>
#include <poincare/integral.h>
#include <escher/palette.h>
#include <ion/unicode/utf8_decoder.h>
#include <float.h>
#include <cmath>

using namespace Poincare;

namespace Shared {

void StorageCartesianFunction::DefaultName(char buffer[], size_t bufferSize) {
  constexpr int k_maxNumberOfDefaultLetterNames = 4;
  static constexpr const char k_defaultLetterNames[k_maxNumberOfDefaultLetterNames] = {
    'f', 'g', 'h', 'p'
  };
  /* First default names are f, g, h, p and then f0, f1... ie, "f[number]",
   * for instance "f12", that does not exist yet in the storage. */
  size_t constantNameLength = 1; // 'f', no null-terminating char
  assert(bufferSize > constantNameLength+1);
  // Find the next available name
  int currentNumber = -k_maxNumberOfDefaultLetterNames;
  int currentNumberLength = 0;
  int availableBufferSize = bufferSize - constantNameLength;
  while (currentNumberLength < availableBufferSize) {
    // Choose letter
    buffer[0] = currentNumber < 0 ? k_defaultLetterNames[k_maxNumberOfDefaultLetterNames+currentNumber] : k_defaultLetterNames[0];
    // Choose number if required
    if (currentNumber >= 0) {
      currentNumberLength = Poincare::Integer(currentNumber).serialize(&buffer[1], availableBufferSize);
    } else {
      buffer[1] = 0;
    }
    if (GlobalContext::SymbolAbstractNameIsFree(buffer)) {
      // Name found
      break;
    }
    currentNumber++;
  }
  assert(currentNumberLength >= 0 && currentNumberLength < availableBufferSize);
}

StorageCartesianFunction StorageCartesianFunction::NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName) {
  static int s_colorIndex = 0;
  // Create the record
  char nameBuffer[SymbolAbstract::k_maxNameSize];
  int numberOfColors = sizeof(Palette::DataColor)/sizeof(KDColor);
  CartesianFunctionRecordData data(Palette::DataColor[s_colorIndex++ % numberOfColors]);
  if (baseName == nullptr) {
    DefaultName(nameBuffer, SymbolAbstract::k_maxNameSize);
    baseName = nameBuffer;
  }
  *error = Ion::Storage::sharedStorage()->createRecordWithExtension(baseName, Ion::Storage::funcExtension, &data, sizeof(data));

  // Return if error
  if (*error != Ion::Storage::Record::ErrorStatus::None) {
    return StorageCartesianFunction();
  }

  // Return the StorageCartesianFunction withthe new record
  return StorageCartesianFunction(Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(baseName, Ion::Storage::funcExtension));
}

int StorageCartesianFunction::derivativeNameWithArgument(char * buffer, size_t bufferSize, char arg) {
  // Fill buffer with f(x). Keep one char foderivativeSizer derivative sign.
  int derivativeSize = UTF8Decoder::CharSizeOfCodePoint('\'');
  int numberOfChars = nameWithArgument(buffer, bufferSize - derivativeSize, arg);
  assert(numberOfChars + derivativeSize < bufferSize);
  char * firstParenthesis = const_cast<char *>(UTF8Helper::CodePointSearch(buffer, '('));
  if (!UTF8Helper::CodePointIs(firstParenthesis, '(')) {
    return numberOfChars;
  }
  memmove(firstParenthesis + derivativeSize, firstParenthesis, buffer + numberOfChars - firstParenthesis);
  UTF8Decoder::CodePointToChars('\'', firstParenthesis, derivativeSize);
  return numberOfChars + derivativeSize;
}

bool StorageCartesianFunction::displayDerivative() const {
  return recordData()->displayDerivative();
}

void StorageCartesianFunction::setDisplayDerivative(bool display) {
  return recordData()->setDisplayDerivative(display);
}

double StorageCartesianFunction::approximateDerivative(double x, Poincare::Context * context) const {
  Poincare::Derivative derivative = Poincare::Derivative::Builder(expressionReduced(context).clone(), Symbol::Builder(Symbol::SpecialSymbols::UnknownX), Poincare::Float<double>::Builder(x)); // derivative takes ownership of Poincare::Float<double>::Builder(x) and the clone of expression
  /* TODO: when we approximate derivative, we might want to simplify the
   * derivative here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(derivative, *context);
}

double StorageCartesianFunction::sumBetweenBounds(double start, double end, Poincare::Context * context) const {
  // TODO: this does not work yet because integral does not understand UnknownX
  Poincare::Integral integral = Poincare::Integral::Builder(expressionReduced(context).clone(), Symbol::Builder(Symbol::SpecialSymbols::UnknownX), Poincare::Float<double>::Builder(start), Poincare::Float<double>::Builder(end)); // Integral takes ownership of args
  /* TODO: when we approximate integral, we might want to simplify the integral
   * here. However, we might want to do it once for all x (to avoid lagging in
   * the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(integral, *context);
}

Expression::Coordinate2D StorageCartesianFunction::nextMinimumFrom(double start, double step, double max, Context * context) const {
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return PoincareHelpers::NextMinimum(expressionReduced(context), unknownX, start, step, max, *context);
}

Expression::Coordinate2D StorageCartesianFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return PoincareHelpers::NextMaximum(expressionReduced(context), unknownX, start, step, max, *context);
}

double StorageCartesianFunction::nextRootFrom(double start, double step, double max, Context * context) const {
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return PoincareHelpers::NextRoot(expressionReduced(context), unknownX, start, step, max, *context);
}

Expression::Coordinate2D StorageCartesianFunction::nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, Expression e) const {
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return PoincareHelpers::NextIntersection(expressionReduced(context), unknownX, start, step, max, *context, e);
}

void * StorageCartesianFunction::Handle::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer+sizeof(CartesianFunctionRecordData);
}

size_t StorageCartesianFunction::Handle::expressionSize(const Ion::Storage::Record * record) const {
  return record->value().size-sizeof(CartesianFunctionRecordData);
}

StorageCartesianFunction::CartesianFunctionRecordData * StorageCartesianFunction::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<CartesianFunctionRecordData *>(const_cast<void *>(d.buffer));
}

}
