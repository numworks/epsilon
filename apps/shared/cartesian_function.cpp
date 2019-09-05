#include "cartesian_function.h"
#include "expression_model_store.h"
#include "poincare_helpers.h"
#include <poincare/derivative.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <escher/palette.h>
#include <ion/unicode/utf8_decoder.h>
#include <apps/i18n.h>
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

Poincare::Expression CartesianFunction::expressionReduced(Poincare::Context * context) const {
  Poincare::Expression result = ExpressionModelHandle::expressionReduced(context);
  if (plotType() == PlotType::Parametric && (
      result.type() != Poincare::ExpressionNode::Type::Matrix ||
      static_cast<Poincare::Matrix&>(result).numberOfRows() != 2 ||
      static_cast<Poincare::Matrix&>(result).numberOfColumns() != 1)
     ) {
    return Poincare::Expression::Parse("[[undef][undef]]");
  }
  return result;
}

I18n::Message CartesianFunction::parameterMessageName() const {
  return ParameterMessageForPlotType(plotType());
}

CodePoint CartesianFunction::symbol() const {
  switch (plotType()) {
  case PlotType::Cartesian:
    return 'x';
  case PlotType::Polar:
    return UCodePointGreekSmallLetterTheta;
  default:
    assert(plotType() == PlotType::Parametric);
    return 't';
  }
}

CartesianFunction::PlotType CartesianFunction::plotType() const {
  return recordData()->plotType();
}

void CartesianFunction::setPlotType(PlotType newPlotType) {
  PlotType currentPlotType = plotType();
  if (newPlotType == currentPlotType) {
    return;
  }
  /* Reset memoized layout. */
  Expression e = expressionClone();
  m_model.tidy();
  double tMin = newPlotType == PlotType::Cartesian ? -INFINITY : 0.0;
  double tMax = newPlotType == PlotType::Cartesian ? INFINITY : 2.0*M_PI;
  setTMin(tMin);
  setTMax(tMax);
  recordData()->setPlotType(newPlotType);
  if (currentPlotType == PlotType::Parametric) {
    // Change [x(t) y(t)] to y(t)
    if (!e.isUninitialized()
        && e.type() == ExpressionNode::Type::Matrix
        && static_cast<Poincare::Matrix&>(e).numberOfRows() == 2
        && static_cast<Poincare::Matrix&>(e).numberOfColumns() == 1)
    {
      Expression nextContent = e.childAtIndex(1);
      /* We need to detach it, otherwise nextContent will think it has a parent
       * when we retrieve it from the storage. */
      nextContent.detachFromParent();
      setExpressionContent(nextContent);
    }
    return;
  } else if (newPlotType == PlotType::Parametric) {
    // Change y(t) to [t y(t)]
    Matrix newExpr = Matrix::Builder();
    newExpr.addChildAtIndexInPlace(Symbol::Builder(UCodePointUnknownX), 0, 0);
    // if y(t) was not uninitialized, insert [t 2t] to set an example
    e = e.isUninitialized() ? Multiplication::Builder(Rational::Builder(2), Symbol::Builder(UCodePointUnknownX)) : e;
    newExpr.addChildAtIndexInPlace(e, newExpr.numberOfChildren(), newExpr.numberOfChildren());
    newExpr.setDimensions(2, 1);
    setExpressionContent(newExpr);
  }
}

I18n::Message CartesianFunction::ParameterMessageForPlotType(PlotType plotType) {
  if (plotType == PlotType::Cartesian) {
    return I18n::Message::X;
  }
  if (plotType == PlotType::Polar) {
    return I18n::Message::Theta;
  }
  assert(plotType == PlotType::Parametric);
  return I18n::Message::T;
}

template <typename T>
Poincare::Coordinate2D<T> CartesianFunction::privateEvaluateXYAtParameter(T t, Poincare::Context * context) const {
  Coordinate2D<T> x1x2 = templatedApproximateAtParameter(t, context);
  PlotType type = plotType();
  if (type == PlotType::Cartesian || type == PlotType::Parametric) {
    return x1x2;
  }
  assert(type == PlotType::Polar);
  T factor = (T)1.0;
  Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  if (angleUnit == Preferences::AngleUnit::Degree) {
    factor = (T) (M_PI/180.0);
  } else if (angleUnit == Preferences::AngleUnit::Gradian) {
    factor = (T) (M_PI/200.0);
  } else {
    assert(angleUnit == Preferences::AngleUnit::Radian);
  }
  const float angle = x1x2.x1()*factor;
  return Coordinate2D<T>(x1x2.x2() * std::cos(angle), x1x2.x2() * std::sin(angle));
}

bool CartesianFunction::displayDerivative() const {
  return recordData()->displayDerivative();
}

void CartesianFunction::setDisplayDerivative(bool display) {
  return recordData()->setDisplayDerivative(display);
}

int CartesianFunction::printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context) {
  PlotType type = plotType();
  if (type == PlotType::Cartesian) {
    return Function::printValue(cursorT, cursorX, cursorY, buffer, bufferSize, precision, context);
  }
  if (type == PlotType::Polar) {
    return PoincareHelpers::ConvertFloatToText<double>(evaluate2DAtParameter(cursorT, context).x2(), buffer, bufferSize, precision);
  }
  assert(type == PlotType::Parametric);
  int result = 0;
  result += UTF8Decoder::CodePointToChars('(', buffer+result, bufferSize-result);
  result += PoincareHelpers::ConvertFloatToText<double>(cursorX, buffer+result, bufferSize-result, precision);
  result += UTF8Decoder::CodePointToChars(';', buffer+result, bufferSize-result);
  result += PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer+result, bufferSize-result, precision);
  result += UTF8Decoder::CodePointToChars(')', buffer+result, bufferSize-result);
  return result;
}

double CartesianFunction::approximateDerivative(double x, Poincare::Context * context) const {
  Poincare::Derivative derivative = Poincare::Derivative::Builder(expressionReduced(context).clone(), Symbol::Builder(UCodePointUnknownX), Poincare::Float<double>::Builder(x)); // derivative takes ownership of Poincare::Float<double>::Builder(x) and the clone of expression
  /* TODO: when we approximate derivative, we might want to simplify the
   * derivative here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(derivative, context);
}

float CartesianFunction::tMin() const {
  return recordData()->tMin();
}

float CartesianFunction::tMax() const {
  return recordData()->tMax();
}

void CartesianFunction::setTMin(float tMin) {
  recordData()->setTMin(tMin);
}

void CartesianFunction::setTMax(float tMax) {
  recordData()->setTMax(tMax);
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
Coordinate2D<T> CartesianFunction::templatedApproximateAtParameter(T t, Poincare::Context * context) const {
  if (isCircularlyDefined(context) || t < tMin() || t > tMax()) {
    return Coordinate2D<T>(NAN, NAN);
  }
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknown[bufferSize];
  Poincare::SerializationHelper::CodePoint(unknown, bufferSize, UCodePointUnknownX);
  PlotType type = plotType();
  if (type == PlotType::Cartesian || type == PlotType::Polar) {
    return Coordinate2D<T>(t, PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(context), unknown, t, context));
  }
  assert(type == PlotType::Parametric);
  Expression e = expressionReduced(context);
  assert(e.type() == ExpressionNode::Type::Matrix);
  assert(static_cast<Poincare::Matrix&>(e).numberOfRows() == 2);
  assert(static_cast<Poincare::Matrix&>(e).numberOfColumns() == 1);
  return Coordinate2D<T>(
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(0), unknown, t, context),
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(1), unknown, t, context));
}

template Coordinate2D<float> CartesianFunction::templatedApproximateAtParameter<float>(float, Poincare::Context *) const;
template Coordinate2D<double> CartesianFunction::templatedApproximateAtParameter<double>(double, Poincare::Context *) const;

}
