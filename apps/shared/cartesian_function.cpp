#include "cartesian_function.h"
#include "expression_model_store.h"
#include "poincare_helpers.h"
#include <poincare/derivative.h>
#include <poincare/integral.h>
#include <poincare/serialization_helper.h>
#include <escher/palette.h>
#include <ion/unicode/utf8_decoder.h>
#include <float.h>
#include <cmath>

using namespace Poincare;

namespace Shared {

void CartesianFunction::DefaultName(char buffer[], size_t bufferSize) {
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

CartesianFunction CartesianFunction::NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName) {
  static int s_colorIndex = 0;
  // Create the record
  char nameBuffer[SymbolAbstract::k_maxNameSize];
  int numberOfColors = sizeof(Palette::DataColor)/sizeof(KDColor);
  CartesianFunctionRecordDataBuffer data(Palette::DataColor[s_colorIndex++ % numberOfColors]);
  if (baseName == nullptr) {
    DefaultName(nameBuffer, SymbolAbstract::k_maxNameSize);
    baseName = nameBuffer;
  }
  *error = Ion::Storage::sharedStorage()->createRecordWithExtension(baseName, Ion::Storage::funcExtension, &data, sizeof(data));

  // Return if error
  if (*error != Ion::Storage::Record::ErrorStatus::None) {
    return CartesianFunction();
  }

  // Return the CartesianFunction withthe new record
  return CartesianFunction(Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(baseName, Ion::Storage::funcExtension));
}

int CartesianFunction::derivativeNameWithArgument(char * buffer, size_t bufferSize) {
  // Fill buffer with f(x). Keep size for derivative sign.
  int derivativeSize = UTF8Decoder::CharSizeOfCodePoint('\'');
  int numberOfChars = nameWithArgument(buffer, bufferSize - derivativeSize);
  assert(numberOfChars + derivativeSize < (int)bufferSize);
  char * firstParenthesis = const_cast<char *>(UTF8Helper::CodePointSearch(buffer, '('));
  if (!UTF8Helper::CodePointIs(firstParenthesis, '(')) {
    return numberOfChars;
  }
  memmove(firstParenthesis + derivativeSize, firstParenthesis, numberOfChars - (firstParenthesis - buffer) + 1);
  UTF8Decoder::CodePointToChars('\'', firstParenthesis, derivativeSize);
  return numberOfChars + derivativeSize;
}

bool CartesianFunction::displayDerivative() const {
  return recordData()->displayDerivative();
}

void CartesianFunction::setDisplayDerivative(bool display) {
  return recordData()->setDisplayDerivative(display);
}

double CartesianFunction::approximateDerivative(double x, Poincare::Context * context) const {
  Poincare::Derivative derivative = Poincare::Derivative::Builder(expressionReduced(context).clone(), Symbol::Builder(UCodePointUnknownX), Poincare::Float<double>::Builder(x)); // derivative takes ownership of Poincare::Float<double>::Builder(x) and the clone of expression
  /* TODO: when we approximate derivative, we might want to simplify the
   * derivative here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(derivative, context);
}

double CartesianFunction::sumBetweenBounds(double start, double end, Poincare::Context * context) const {
  // TODO: this does not work yet because integral does not understand UnknownX
  Poincare::Integral integral = Poincare::Integral::Builder(expressionReduced(context).clone(), Symbol::Builder(UCodePointUnknownX), Poincare::Float<double>::Builder(start), Poincare::Float<double>::Builder(end)); // Integral takes ownership of args
  /* TODO: when we approximate integral, we might want to simplify the integral
   * here. However, we might want to do it once for all x (to avoid lagging in
   * the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(integral, context);
}

Coordinate2D CartesianFunction::nextMinimumFrom(double start, double step, double max, Context * context) const {
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknownX);
  return PoincareHelpers::NextMinimum(expressionReduced(context), unknownX, start, step, max, context);
}

Coordinate2D CartesianFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknownX);
  return PoincareHelpers::NextMaximum(expressionReduced(context), unknownX, start, step, max, context);
}

double CartesianFunction::nextRootFrom(double start, double step, double max, Context * context) const {
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknownX);
  return PoincareHelpers::NextRoot(expressionReduced(context), unknownX, start, step, max, context);
}

Coordinate2D CartesianFunction::nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, Expression e) const {
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknownX);
  return PoincareHelpers::NextIntersection(expressionReduced(context), unknownX, start, step, max, context, e);
}

void * CartesianFunction::Model::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer+sizeof(CartesianFunctionRecordDataBuffer);
}

size_t CartesianFunction::Model::expressionSize(const Ion::Storage::Record * record) const {
  return record->value().size-sizeof(CartesianFunctionRecordDataBuffer);
}

CartesianFunction::CartesianFunctionRecordDataBuffer * CartesianFunction::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<CartesianFunctionRecordDataBuffer *>(const_cast<void *>(d.buffer));
}

template<typename T>
T CartesianFunction::templatedApproximateAtAbscissa(T x, Poincare::Context * context) const {
  if (isCircularlyDefined(context)) {
    return NAN;
  }
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  Poincare::SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknownX);
  return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(context), unknownX, x, context);
}

template float CartesianFunction::templatedApproximateAtAbscissa<float>(float, Poincare::Context *) const;
template double CartesianFunction::templatedApproximateAtAbscissa<double>(double, Poincare::Context *) const;

}
