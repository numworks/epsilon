#include "continuous_function.h"
#include <escher/palette.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/polynomial.h>
#include <poincare/zoom.h>
#include <poincare/integral.h>
#include <poincare/function.h>
#include <poincare/float.h>
#include <poincare/matrix.h>
#include <poincare/symbol_abstract.h>
#include <poincare/serialization_helper.h>
#include <poincare/trigonometry.h>
#include <poincare/comparison_operator.h>
#include <poincare/derivative.h>
#include "poincare_helpers.h"
#include <algorithm>
#include "global_context.h"

using namespace Poincare;

namespace Shared {

constexpr char ContinuousFunction::k_unknownName[2];
constexpr char ContinuousFunction::k_ordinateName[2];

/* ContinuousFunction - Public */

ContinuousFunction ContinuousFunction::NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName) {
  static int s_colorIndex = 0;
  assert(baseName != nullptr);
  // Create the record
  RecordDataBuffer data(Escher::Palette::nextDataColor(&s_colorIndex));
  *error = Ion::Storage::sharedStorage()->createRecordWithExtension(baseName, Ion::Storage::funcExtension, &data, sizeof(data));
  if (*error != Ion::Storage::Record::ErrorStatus::None) {
    return ContinuousFunction();
  }
  // Return the ContinuousFunction with the new record
  return ContinuousFunction(Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(baseName, Ion::Storage::funcExtension));
}

I18n::Message ContinuousFunction::MessageForSymbolType(SymbolType symbolType) {
  switch (symbolType) {
  case SymbolType::T:
    return I18n::Message::T;
  case SymbolType::Theta:
    return I18n::Message::Theta;
  default:
    return I18n::Message::X;
  }
}

ContinuousFunction::SymbolType ContinuousFunction::symbolType() const {
  PlotType functionPlotType = plotType();
  return functionPlotType >= PlotType::Cartesian ? SymbolType::X : static_cast<SymbolType>(functionPlotType);
}

I18n::Message ContinuousFunction::plotTypeMessage() const {
  const size_t plotTypeIndex = static_cast<size_t>(plotType());
  assert(plotTypeIndex < k_numberOfPlotTypes);
  constexpr I18n::Message category[k_numberOfPlotTypes] = {
      I18n::Message::PolarType,          I18n::Message::ParametricType,
      I18n::Message::CartesianType,      I18n::Message::LineType,
      I18n::Message::HorizontalLineType, I18n::Message::VerticalLineType,
      I18n::Message::InequationType,     I18n::Message::CircleType,
      I18n::Message::EllipseType,        I18n::Message::ParabolaType,
      I18n::Message::HyperbolaType,      I18n::Message::OtherType,
      I18n::Message::UndefinedType,      I18n::Message::UnhandledType,
  };
  return category[plotTypeIndex];
}

CodePoint ContinuousFunction::symbol() const {
  switch (symbolType()) {
  case SymbolType::T:
    return 't';
  case SymbolType::Theta:
    return UCodePointGreekSmallLetterTheta;
  default:
    return 'x';
  }
}

int ContinuousFunction::nameWithArgument(char * buffer, size_t bufferSize) {
  if (isNamed()) {
    return Function::nameWithArgument(buffer, bufferSize);
  }
  return strlcpy(buffer, k_ordinateName, bufferSize);
}

int ContinuousFunction::printValue(int index, double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Context * context) {
  if (index == 0) {
    /* With Vertical curves, cursorT != cursorX .
     * We need the value for symbol=... */
    return PoincareHelpers::ConvertFloatToText<double>(isAlongX() ? cursorX : cursorT, buffer, bufferSize, precision);
  }

  PlotType type = plotType();
  if (type == PlotType::Parametric) {
    int result = 0;
    result += UTF8Decoder::CodePointToChars('(', buffer+result, bufferSize-result);
    result += PoincareHelpers::ConvertFloatToText<double>(cursorX, buffer+result, bufferSize-result, precision);
    result += UTF8Decoder::CodePointToChars(';', buffer+result, bufferSize-result);
    result += PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer+result, bufferSize-result, precision);
    result += UTF8Decoder::CodePointToChars(')', buffer+result, bufferSize-result);
    return result;
  }
  if (type == PlotType::Polar) {
    return PoincareHelpers::ConvertFloatToText<double>(evaluate2DAtParameter(cursorT, context).x2(), buffer, bufferSize, precision);
  }
  return PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer, bufferSize, precision);
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::setContent(const char * c, Context * context) {
  Ion::Storage::Record::ErrorStatus error = ExpressionModelHandle::setContent(c, context);
  if (error == Ion::Storage::Record::ErrorStatus::None && !isNull()) {
    udpateModel(context);
    error = editableModel()->renameRecordIfNeeded(this, c, context, symbol());
  }
  return error;
}

bool ContinuousFunction::drawDottedCurve() const {
  ExpressionNode::Type eqSymbol = equationSymbol();
  return eqSymbol == ExpressionNode::Type::Superior || eqSymbol == ExpressionNode::Type::Inferior;
}

bool ContinuousFunction::drawInferiorArea() const {
  ExpressionNode::Type eqSymbol = equationSymbol();
  return eqSymbol == ExpressionNode::Type::Inferior || eqSymbol == ExpressionNode::Type::InferiorEqual;
}

bool ContinuousFunction::drawSuperiorArea() const {
  ExpressionNode::Type eqSymbol = equationSymbol();
  return eqSymbol == ExpressionNode::Type::Superior || eqSymbol == ExpressionNode::Type::SuperiorEqual;
}

bool ContinuousFunction::isNamed() const {
  // Unnamed functions have a fullname starting with '?'
  const char * recordFullName = fullName();
  return recordFullName != nullptr && recordFullName[0] != '?';
}

void ContinuousFunction::getLineParameters(double * slope, double * intercept, Poincare::Context * context) const {
  assert(plotType() == PlotType::Line);
  Expression equation = expressionReduced(context);
  // Compute metrics for details view of Line
  Preferences::ComplexFormat complexFormat = Preferences::sharedPreferences()->complexFormat();
  Poincare::Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Reduce to another target for coefficient analysis.
  // TODO Hugo : Ensure reducing again is unnecessary
  // PoincareHelpers::Reduce(&equation, context,
  //                         ExpressionNode::ReductionTarget::SystemForAnalysis);
  // Separate the two line coefficients
  int d = equation.getPolynomialReducedCoefficients(
      k_unknownName, coefficients, context, complexFormat, angleUnit,
      k_defaultUnitFormat, k_defaultSymbolicComputation);
  assert(d == 1);
  (void) d;  // Silence compilation warnings
  // Approximate and return the two line coefficients
  *slope = coefficients[1].approximateToScalar<double>(context, complexFormat, angleUnit);
  *intercept = coefficients[0].approximateToScalar<double>(context, complexFormat, angleUnit);
}

Poincare::Conic ContinuousFunction::getConicParameters(Poincare::Context * context) const {
  assert(isConic());
  return Conic(expressionEquation(context), context, k_unknownName);
}

void ContinuousFunction::udpateModel(Context * context) {
  bool previousAlongXStatus = isAlongX();
  updatePlotType(context);
  if (previousAlongXStatus != isAlongX()) {
    // isAlongX status has changed, reset the definition's domain
    setTMin(!isAlongX() ? 0.0 : -INFINITY);
    setTMax(!isAlongX()
                ? 2.0 * Trigonometry::PiInAngleUnit(
                            Preferences::sharedPreferences()->angleUnit())
                : INFINITY);
  }
}

int ContinuousFunction::derivativeNameWithArgument(char * buffer, size_t bufferSize) {
  if (!isNamed()) {
    return strlcpy(buffer, "y'", bufferSize);
  }
  const CodePoint derivative = '\'';
  int derivativeSize = UTF8Decoder::CharSizeOfCodePoint(derivative);
  // Fill buffer with f(x). Keep size for derivative sign.
  int numberOfChars = nameWithArgument(buffer, bufferSize - derivativeSize);
  assert(numberOfChars + derivativeSize < (int)bufferSize);
  // Find (x)
  char * firstParenthesis = const_cast<char *>(UTF8Helper::CodePointSearch(buffer, '('));
  assert(UTF8Helper::CodePointIs(firstParenthesis, '('));
  // Move parentheses to fit derivative CodePoint
  memmove(firstParenthesis + derivativeSize, firstParenthesis, numberOfChars - (firstParenthesis - buffer) + 1);
  // Insert derivative CodePoint
  UTF8Decoder::CodePointToChars(derivative, firstParenthesis, derivativeSize);
  return numberOfChars + derivativeSize;
}

double ContinuousFunction::approximateDerivative(double x, Context * context, int secondaryCurveIndex) const {
  assert(isAlongX());
  PlotType type = plotType();
  if (x < tMin() || x > tMax() || type == PlotType::VerticalLine) {
    return NAN;
  }
  // Derivative is simplified once and for all
  Expression derivate = expressionDerivateReduced(context);
  if (hasTwoCurves()) {
      assert(derivate.type() == Poincare::ExpressionNode::Type::Dependency);
      derivate = derivate.childAtIndex(0);
      assert(derivate.type() == Poincare::ExpressionNode::Type::Matrix);
      derivate = derivate.childAtIndex(secondaryCurveIndex);
  } else {
    assert(secondaryCurveIndex == 0);
  }
  return PoincareHelpers::ApproximateWithValueForSymbol(derivate, k_unknownName, x, context);
}

void ContinuousFunction::setTMin(float tMin) {
  recordData()->setTMin(tMin);
  // TODO Hugo : Re-check cache
  // setCache(nullptr);
}

void ContinuousFunction::setTMax(float tMax) {
  recordData()->setTMax(tMax);
  // TODO Hugo : Re-check cache
  // setCache(nullptr);
}

bool ContinuousFunction::basedOnCostlyAlgorithms(Context * context) const {
  return expressionReduced(context).hasExpression([](const Expression e, const void * context) {
      return e.type() == ExpressionNode::Type::Sequence
          || e.type() == ExpressionNode::Type::Integral
          || e.type() == ExpressionNode::Type::Derivative;
      }, nullptr);
}

void ContinuousFunction::xRangeForDisplay(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Context * context) const {
  if (!isAlongX()) {
    assert(std::isfinite(tMin()) && std::isfinite(tMax()) && std::isfinite(rangeStep()) && rangeStep() > 0);
    assert(!hasTwoCurves());
    protectedFullRangeForDisplay(tMin(), tMax(), rangeStep(), xMin, xMax, context, true);
    *yMinIntrinsic = FLT_MAX;
    *yMaxIntrinsic = -FLT_MAX;
    return;
  }

  *xMin = NAN;
  *xMax = NAN;
  *yMinIntrinsic = NAN;
  *yMaxIntrinsic = NAN;

  if (basedOnCostlyAlgorithms(context)) {
    /* The function makes use of some costly algorithms, such as integration or
     * sequences, and cannot be computed in a timely manner. */
    return;
  }

  Zoom::ValueAtAbscissa evaluation = [](float x, Context * context, const void * auxiliary) {
    /* When evaluating sin(x)/x close to zero using the standard sine function,
     * one can detect small variations, while the cardinal sine is supposed to
     * be locally monotonous. To smooth out such variations, we round the
     * result of the evaluations. As we are not interested in precise results
     * but only in ordering, this approximation is sufficient. */
    constexpr float precision = 1e-5;
    return precision * std::round(static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context, 0).x2() / precision);
  };
  Zoom::InterestingRangesForDisplay(evaluation, xMin, xMax, yMinIntrinsic, yMaxIntrinsic, std::max(tMin(), xMinLimit), std::min(tMax(), xMaxLimit), context, this);

  if (hasTwoCurves()) {
    // Temporarily store previous results
    float xMinTemp = *xMin;
    float xMaxTemp = *xMax;
    float yMinTemp = *yMinIntrinsic;
    float yMaxTemp = *yMaxIntrinsic;
    // Reset values
    *xMin = NAN;
    *xMax = NAN;
    *yMinIntrinsic = NAN;
    *yMaxIntrinsic = NAN;
    // Compute interesting ranges for the second curve as well.
    Zoom::ValueAtAbscissa secondCurveEvaluation = [](float x, Context * context, const void * auxiliary) {
      // See comment in previous evaluation
      constexpr float precision = 1e-5;
      return precision * std::round(static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context, 1).x2() / precision);
    };
    Zoom::InterestingRangesForDisplay(secondCurveEvaluation, xMin, xMax, yMinIntrinsic, yMaxIntrinsic, std::max(tMin(), xMinLimit), std::min(tMax(), xMaxLimit), context, this);
    // Combine the resulting ranges
    Zoom::CombineRanges(xMinTemp, xMaxTemp, *xMin, *xMax, xMin, xMax);
    Zoom::CombineRanges(yMinTemp, yMaxTemp, *yMinIntrinsic, *yMaxIntrinsic, yMinIntrinsic, yMaxIntrinsic);
  }
}

void ContinuousFunction::yRangeForDisplay(float xMin, float xMax, float yMinForced, float yMaxForced, float ratio, float * yMin, float * yMax, Context * context, bool optimizeRange) const {
  if (!isAlongX()) {
    assert(std::isfinite(tMin()) && std::isfinite(tMax()) && std::isfinite(rangeStep()) && rangeStep() > 0);
    assert(!hasTwoCurves());
    protectedFullRangeForDisplay(tMin(), tMax(), rangeStep(), yMin, yMax, context, false);
    return;
  }

  *yMin = NAN;
  *yMax = NAN;

  if (basedOnCostlyAlgorithms(context)) {
    /* The function makes use of some costly algorithms, such as integration or
     * sequences, and cannot be computed in a timely manner. */
    return;
  }

  if (!optimizeRange) {
    // TODO Hugo : Adapt protectedFullRangeForDisplay for functions with two curves
    protectedFullRangeForDisplay(xMin, xMax, (xMax - xMin) / (Ion::Display::Width / 4), yMin, yMax, context, false);
    return;
  }

  Zoom::ValueAtAbscissa evaluation = [](float x, Context * context, const void * auxiliary) {
    return static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context, 0).x2();
  };

  /* TODO Hugo : yRangeForDisplay currently doesn't support ContinuousFunctions
   * with multiple curves. For that, RangeWithRatioForDisplay should be changed
   * to handle a second evaluation. In the meantime, all ContinuousFunctions
   * having two curves are displayed orthonormal. */
  if (yMaxForced - yMinForced <= ratio * (xMax - xMin) && !hasTwoCurves()) {
    Zoom::RangeWithRatioForDisplay(evaluation, ratio, xMin, xMax, yMinForced, yMaxForced, yMin, yMax, context, this);
    // if (hasTwoCurves()) {
    //   float yMinTemp = *yMin;
    //   float yMaxTemp = *yMax;
    //   *yMin = NAN;
    //   *yMax = NAN;
    //   Zoom::ValueAtAbscissa secondCurveEvaluation = [](float x, Context * context, const void * auxiliary) {
    //     return static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context, 1).x2();
    //   };
    //   Zoom::RangeWithRatioForDisplay(secondCurveEvaluation, ratio, xMin, xMax, yMinForced, yMaxForced, yMin, yMax, context, this);
    //   Zoom::CombineRanges(yMinTemp, yMaxTemp, *yMin, *yMax, yMin, yMax);
    //   assert(false); // Ratio is no longer respected.
    // }
    if (*yMin < *yMax) {
      return;
    }
    *yMin = NAN;
    *yMax = NAN;
  }

  Zoom::RefinedYRangeForDisplay(evaluation, xMin, xMax, yMin, yMax, context, this);

  if (hasTwoCurves()) {
    float yMinTemp = *yMin;
    float yMaxTemp = *yMax;
    *yMin = NAN;
    *yMax = NAN;

    Zoom::ValueAtAbscissa secondCurveEvaluation = [](float x, Context * context, const void * auxiliary) {
      return static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context, 1).x2();
    };
    Zoom::RefinedYRangeForDisplay(secondCurveEvaluation, xMin, xMax, yMin, yMax, context, this);

    Zoom::CombineRanges(yMinTemp, yMaxTemp, *yMin, *yMax, yMin, yMax);
  }
}

Coordinate2D<double> ContinuousFunction::nextMinimumFrom(double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) const {
  return nextPointOfInterestFrom(start, max, context, [](Expression e, const char * symbol, double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) { return PoincareHelpers::NextMinimum(e, symbol, start, max, context, relativePrecision, minimalStep, maximalStep); }, relativePrecision, minimalStep, maximalStep);
}

Coordinate2D<double> ContinuousFunction::nextMaximumFrom(double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) const {
  return nextPointOfInterestFrom(start, max, context, [](Expression e, const char * symbol, double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) { return PoincareHelpers::NextMaximum(e, symbol, start, max, context, relativePrecision, minimalStep, maximalStep); }, relativePrecision, minimalStep, maximalStep);
}

Coordinate2D<double> ContinuousFunction::nextRootFrom(double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) const {
  return nextPointOfInterestFrom(start, max, context, [](Expression e, const char * symbol, double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) { return Coordinate2D<double>(PoincareHelpers::NextRoot(e, symbol, start, max, context, relativePrecision, minimalStep, maximalStep), 0.0); }, relativePrecision, minimalStep, maximalStep);
}

Coordinate2D<double> ContinuousFunction::nextIntersectionFrom(double start, double max, Context * context, Expression e, double relativePrecision, double minimalStep, double maximalStep, double eDomainMin, double eDomainMax) const {
  assert(isAlongX());
  double tmin = std::max<double>(tMin(), eDomainMin), tmax = std::min<double>(tMax(), eDomainMax);
  start = start < tmin ? tmin : start > tmax ? tmax : start;
  max = max < tmin ? tmin : max > tmax ? tmax : max;
  if (start == max) {
    return NAN;
  }
  return PoincareHelpers::NextIntersection(expressionReduced(context), k_unknownName, start, max, context, e, relativePrecision, minimalStep, maximalStep);
}

Expression ContinuousFunction::sumBetweenBounds(double start, double end, Context * context) const {
  assert(isAlongX());
  start = std::max<double>(start, tMin());
  end = std::min<double>(end, tMax());
  return Integral::Builder(expressionReduced(context).clone(), Symbol::Builder(UCodePointUnknown), Float<double>::Builder(start), Float<double>::Builder(end)); // Integral takes ownership of args
  /* TODO: when we approximate integral, we might want to simplify the integral
   * here. However, we might want to do it once for all x (to avoid lagging in
   * the derivative table. */
}

/* ContinuousFunction - Private */

float ContinuousFunction::rangeStep() const {
  return isAlongX() ? NAN : (tMax() - tMin())/k_polarParamRangeSearchNumberOfPoints;
}

void ContinuousFunction::updatePlotType(Context * context) {
  // Retrieve ContinuousFunction's equation
  Expression equation = expressionEquation(context);
  // Compute equation's degree regarding x and y.
  int yDeg = equation.polynomialDegree(context, k_ordinateName);
  int xDeg = equation.polynomialDegree(context, k_unknownName);
  // Inequations : equation symbol has been updated when parsing the equation
  recordData()->setEquationSymbol(m_model.m_equationSymbol);
  if (m_model.m_equationSymbol != ExpressionNode::Type::Equal) {
    ExpressionNode::Sign ySign;
    // Inequations are handled with a few constraint on y and x degrees.
    if ((yDeg == 1 || (yDeg == 2 && xDeg > 0)) && isYCoefficientNonNull(equation, yDeg, context, &ySign) && ySign != ExpressionNode::Sign::Unknown) {
      if (ySign == ExpressionNode::Sign::Negative) {
        // Oppose the comparison operator
        Poincare::ExpressionNode::Type newEquationSymbol = ComparisonOperator::Opposite(m_model.m_equationSymbol);
        m_model.m_equationSymbol = newEquationSymbol;
        recordData()->setEquationSymbol(newEquationSymbol);
      }
      return recordData()->setPlotType(PlotType::Inequation);
    } else if (yDeg <= 0 && m_model.m_plotType == PlotType::Cartesian) {
      // Inequation on a named cartesian function
      return recordData()->setPlotType(PlotType::Inequation);
    } else {
      // TODO : Handle vertical lines inequations
      return recordData()->setPlotType(PlotType::Unhandled);
    }
  }
  // TODO Hugo : Improve how named functions are detected
  // Named functions : PlotType has been updated when parsing the equation
  if (m_model.m_plotType != PlotType::Undefined) {
    // There should be no y symbol.
    if (yDeg > 0) {
      return recordData()->setPlotType(PlotType::Unhandled);
    }
    return recordData()->setPlotType(m_model.m_plotType);
  }

  /* We can now rely on x and y degree to identify plot type :
   * | y  | x  | Status
   * | 0  | 0  | Undefined
   * | 0  | 1  | Vertical Line
   * | 0  | +  | Unhandled
   * | 1  | 0  | Horizontal Line
   * | 1  | 1  | Line
   * | 1  | +  | Cartesian
   * | 2  | 0  | Other (Two Horizontal Lines)
   * | 2  | 1  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | 2  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | +  | Other
   * | +  | +  | Unhandled
   */

  if (yDeg == 0 && xDeg == 0) {
    return recordData()->setPlotType(PlotType::Undefined);
  }
  if (yDeg == 0 && xDeg == 1) {
    return recordData()->setPlotType(PlotType::VerticalLine);
  }
  if (yDeg <= 0 || yDeg > 2 || !isYCoefficientNonNull(equation, yDeg, context)) {
    /* Any other case where yDeg is null ,> 2 or negative isn't handled.
     * Same if y's highest degree term depends on x, or may be null. */
    return recordData()->setPlotType(PlotType::Unhandled);
  }
  if (yDeg == 1 && xDeg == 0) {
    return recordData()->setPlotType(PlotType::HorizontalLine);
  }
  if (yDeg == 1 && xDeg == 1) {
    return recordData()->setPlotType(PlotType::Line);
  }
  if (yDeg == 1) {
    return recordData()->setPlotType(PlotType::Cartesian);
  }
  if (yDeg == 2 && (xDeg == 1 || xDeg == 2)) {
    Conic equationConic = Conic(equation, context, k_unknownName);
    Conic::Type ctype = equationConic.getConicType();
    if (ctype == Conic::Type::Hyperbola) {
      return recordData()->setPlotType(PlotType::Hyperbola);
    } else if (ctype == Conic::Type::Parabola) {
      return recordData()->setPlotType(PlotType::Parabola);
    } else if (ctype == Conic::Type::Ellipse) {
      return recordData()->setPlotType(PlotType::Ellipse);
    } else if (ctype == Conic::Type::Circle) {
      return recordData()->setPlotType(PlotType::Circle);
    }
  }
  assert(yDeg == 2);
  // Unknown type that we are able to plot anyway.
  return recordData()->setPlotType(PlotType::Other);
}

bool ContinuousFunction::isYCoefficientNonNull(Poincare::Expression equation, int yDeg, Context * context, Poincare::ExpressionNode::Sign * coefficientSign) const {
  assert(yDeg >= 0 && !equation.isUninitialized());
  if (coefficientSign != nullptr) {
    *coefficientSign = ExpressionNode::Sign::Unknown;
  }
  // Compute y coefficients
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  Preferences::ComplexFormat complexFormat = Preferences::sharedPreferences()->complexFormat();
  Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  int dy = equation.getPolynomialReducedCoefficients(k_ordinateName, coefficients, context, complexFormat, angleUnit, k_defaultUnitFormat, k_defaultSymbolicComputation);
  assert(dy >= yDeg);
  (void) dy; // Silence warnings
  // Retrieve requested coefficient
  ExpressionNode::NullStatus coefficientNullStatus = coefficients[yDeg].nullStatus(context);
  if (coefficientNullStatus == ExpressionNode::NullStatus::Null || coefficients[yDeg].polynomialDegree(context, k_unknownName) != 0) {
    // Coefficient is null or depends on x (which may be null)
    // NOTE : We could handle here cases where it depends on x but is never null
    return false;
  }
  if (coefficientSign != nullptr) {
    *coefficientSign = coefficients[yDeg].sign(context);
  }
  if (coefficientNullStatus == ExpressionNode::NullStatus::NonNull && (coefficientSign == nullptr || *coefficientSign != ExpressionNode::Sign::Unknown)) {
    // Coefficient is non null, sign is either known or disregarded.
    return true;
  }
  // Approximate the coefficient, update sign and nullstatus
  double approximation = coefficients[yDeg].approximateToScalar<double>(context, complexFormat, angleUnit);
  coefficientNullStatus = approximation != 0. ? ExpressionNode::NullStatus::NonNull : ExpressionNode::NullStatus::Null;
  if (coefficientSign != nullptr) {
    *coefficientSign = approximation < 0. ? ExpressionNode::Sign::Negative : ExpressionNode::Sign::Positive;
  }
  return coefficientNullStatus == ExpressionNode::NullStatus::NonNull;
}

Coordinate2D<double> ContinuousFunction::nextPointOfInterestFrom(double start, double max, Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const {
  assert(isAlongX());
  double tmin = tMin(), tmax = tMax();
  start = start < tmin ? tmin : start > tmax ? tmax : start;
  max = max < tmin ? tmin : max > tmax ? tmax : max;
  if (start == max) {
    return NAN;
  }
  return compute(expressionReduced(context), k_unknownName, start, max, context, relativePrecision, minimalStep, maximalStep);
}

template <typename T>
Poincare::Coordinate2D<T> ContinuousFunction::privateEvaluateXYAtParameter(T t, Context * context, int secondaryCurveIndex) const {
  Coordinate2D<T> x1x2 = templatedApproximateAtParameter(t, context, secondaryCurveIndex);
  if (plotType() != PlotType::Polar) {
    return x1x2;
  }
  const T angle = x1x2.x1() * M_PI /
                  Trigonometry::PiInAngleUnit(
                      Preferences::sharedPreferences()->angleUnit());
  return Coordinate2D<T>(x1x2.x2() * std::cos(angle),
                         x1x2.x2() * std::sin(angle));
}

template<typename T>
Coordinate2D<T> ContinuousFunction::templatedApproximateAtParameter(T t, Context * context, int secondaryCurveIndex) const {
  if (t < tMin() || t > tMax()) {
    return Coordinate2D<T>(isAlongX() ? t : NAN, NAN);
  }
  Expression e = expressionReduced(context);
  PlotType type = plotType();
  if (type != PlotType::Parametric) {
    if (hasTwoCurves()) {
      assert(e.numberOfChildren() > secondaryCurveIndex);
      return Coordinate2D<T>(t, PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(secondaryCurveIndex), k_unknownName, t, context));
    } else {
      assert(secondaryCurveIndex == 0);
    }
    if (type == PlotType::VerticalLine) {
      // Invert x and y with vertical lines so it can be scrolled vertically
      return Coordinate2D<T>(PoincareHelpers::ApproximateWithValueForSymbol(e, k_unknownName, t, context), t);
    }
    return Coordinate2D<T>(t, PoincareHelpers::ApproximateWithValueForSymbol(e, k_unknownName, t, context));
  }
  if (e.type() == ExpressionNode::Type::Dependency) {
    e = e.childAtIndex(0);
  }
  assert(e.type() == ExpressionNode::Type::Matrix);
  assert(static_cast<Matrix&>(e).numberOfRows() == 2);
  assert(static_cast<Matrix&>(e).numberOfColumns() == 1);
  return Coordinate2D<T>(
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(0), k_unknownName, t, context),
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(1), k_unknownName, t, context));
}

// TODO Hugo : Sort functions below

Expression ContinuousFunction::Model::originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const {
  return Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record)).replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Symbol::Builder(symbol));
}

Expression ContinuousFunction::Model::expressionEquation(const Ion::Storage::Record * record, Context * context) const {
  // TODO Hugo : Add todo expressionReduced on circularity ?
  // TODO Hugo : Either memoize or limit calls to this method
  // Initialize plot type
  m_plotType = PlotType::Undefined;
  if (record->fullName() != nullptr && record->fullName()[0] != '?' && isCircularlyDefined(record, context)) {
    return Undefined::Builder();
  }
  Expression result = Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
  if (result.isUninitialized()) {
    return Undefined::Builder();
  }
  assert(ComparisonOperator::IsComparisonOperatorType(result.type()));
  m_equationSymbol = result.type();
  bool recordShouldBeNammed = false;
  if (result.childAtIndex(0).type() == ExpressionNode::Type::Function
    && (result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder('x'))
      || (result.type() == ExpressionNode::Type::Equal
        && (result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder(UCodePointGreekSmallLetterTheta))
          || result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder('t')))))) {
    // Expression is of the form f(x)[=/>/<] or f([t/θ])=
    // TODO Hugo : Improve that
    assert(record->fullName() != nullptr);
    Expression exp = result.childAtIndex(0).clone();
    Poincare::Function f = static_cast<Poincare::Function&>(exp);
    const size_t functionNameLength = strlen(f.name());
    if (Shared::GlobalContext::SymbolAbstractNameIsFree(f.name())
        || (record->fullName()[0] != '?'
            && memcmp(record->fullName(), f.name(), functionNameLength) == 0
            && record->fullName()[functionNameLength] == '.')) {
      // Named record : it is either already named, or will soon be renamed.
      if (result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder(UCodePointGreekSmallLetterTheta))) {
        m_plotType = PlotType::Polar;
      } else if (result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder('x'))) {
        m_plotType = PlotType::Cartesian;
      } else {
        m_plotType = PlotType::Parametric;
      }
      result = result.childAtIndex(1);
      recordShouldBeNammed = true;
    } else {
      // Function in first half of the equation refer to an already defined one.
      result.childAtIndex(0).replaceChildAtIndexInPlace(0, Symbol::Builder(UCodePointUnknown));
    }
  }
  if (!recordShouldBeNammed) {
    result = Subtraction::Builder(result.childAtIndex(0), result.childAtIndex(1));
  }
  PoincareHelpers::Reduce(&result, context, ExpressionNode::ReductionTarget::SystemForAnalysis);
  assert(!result.isUninitialized()); // TODO Hugo : Ensure this assert
  // TODO Hugo : Memoize it ?
  return result;
}

Expression ContinuousFunction::Model::expressionReduced(const Ion::Storage::Record * record, Context * context) const {
  // TODO Hugo : Fix this
  // Get expression degree on y
  if (m_expression.isUninitialized()) {
    // Retrieve the expression from the equation
    m_expression = expressionEquation(record, context);
    m_hasTwoCurves = false;
    if (record->fullName() == nullptr || record->fullName()[0] == '?') {
      // Transform the solution by solving the equation in y
      int degree = m_expression.polynomialDegree(context, k_ordinateName);
      if (degree < 0 || degree >= 3) {
        m_expression = Undefined::Builder();
      } else if (degree == 0) {
        // Vertical line
        int xDegree = m_expression.polynomialDegree(context, k_unknownName);
        if (xDegree == 1) {
          Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
          int d = m_expression.getPolynomialReducedCoefficients(k_unknownName, coefficients, context, Preferences::ComplexFormat::Cartesian, Preferences::sharedPreferences()->angleUnit(), k_defaultUnitFormat, k_defaultSymbolicComputation);
          assert(d == xDegree);
          (void) d; // Silence compilation warning
          Expression root;
          Polynomial::LinearPolynomialRoots(coefficients[1], coefficients[0], &root, context, Preferences::ComplexFormat::Real, Preferences::sharedPreferences()->angleUnit());
          m_expression = root;
        } else {
          /* TODO : We could handle equations of any degree by solving the
           * equation within the graph view bounds, to plot as many vertical
           * lines as needed. */
          m_expression = Undefined::Builder();
        }
      } else {
        Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
        int d = m_expression.getPolynomialReducedCoefficients(k_ordinateName, coefficients, context, Preferences::ComplexFormat::Cartesian, Preferences::sharedPreferences()->angleUnit(), k_defaultUnitFormat, k_defaultSymbolicComputation);
        assert(d == degree);
        if (d == 1) {
          Expression root;
          Polynomial::LinearPolynomialRoots(coefficients[1], coefficients[0], &root, context, Preferences::ComplexFormat::Real, Preferences::sharedPreferences()->angleUnit());
          m_expression = root;
        } else {
          Expression root1, root2, delta;
          int solutions = Polynomial::QuadraticPolynomialRoots(coefficients[2], coefficients[1], coefficients[0], &root1, &root2, &delta, context, Preferences::ComplexFormat::Real, Preferences::sharedPreferences()->angleUnit());
          if (solutions <= 1) {
            m_expression = root1;
          } else {
            m_hasTwoCurves = true;
            Matrix newExpr = Matrix::Builder();
            // Roots must be ordered so that the first curve is above the second
            newExpr.addChildAtIndexInPlace(root2, 0, 0);
            newExpr.addChildAtIndexInPlace(root1, 1, 1);
            newExpr.setDimensions(2, 1);
            m_expression = newExpr;
          }
        }
      }
    }
    // Reduce it to optimize approximations
    PoincareHelpers::Reduce(&m_expression, context, ExpressionNode::ReductionTarget::SystemForApproximation);
  }
  return m_expression;
}

Expression ContinuousFunction::Model::expressionClone(const Ion::Storage::Record * record) const {
  assert(record->fullName() != nullptr);
  /* A new Expression has to be created at each call (because it might be tempered with after calling) */
  Expression e = Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
  assert(ComparisonOperator::IsComparisonOperatorType(e.type()));
  return e.childAtIndex(1);
  // TODO Hugo : Maybe perform the substitution with symbol here.
}

void ContinuousFunction::Model::tidy() const {
  m_hasTwoCurves = false;
  m_equationSymbol = Poincare::ExpressionNode::Type::Equal;
  m_plotType = PlotType::Undefined;
  m_expressionDerivate = Expression();
  ExpressionModel::tidy();
}

void * ContinuousFunction::Model::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer+sizeof(RecordDataBuffer);
}

size_t ContinuousFunction::Model::expressionSize(const Ion::Storage::Record * record) const {
  return record->value().size-sizeof(RecordDataBuffer);
}

Expression ContinuousFunction::Model::expressionDerivateReduced(const Ion::Storage::Record * record, Context * context) const {
  if (m_expressionDerivate.isUninitialized()) {
    Expression expression = expressionReduced(record, context).clone();
    if (hasTwoCurves()) {
      Matrix newExpr = Matrix::Builder();
      assert(expression.type() == Poincare::ExpressionNode::Type::Matrix);
      newExpr.addChildAtIndexInPlace(Poincare::Derivative::Builder(expression.childAtIndex(1), Symbol::Builder(UCodePointUnknown), Symbol::Builder(UCodePointUnknown)), 0, 0);
      newExpr.addChildAtIndexInPlace(Poincare::Derivative::Builder(expression.childAtIndex(0), Symbol::Builder(UCodePointUnknown), Symbol::Builder(UCodePointUnknown)), 1, 1);
      newExpr.setDimensions(2, 1);
      m_expressionDerivate = newExpr;
    } else {
      m_expressionDerivate = Poincare::Derivative::Builder(expression, Symbol::Builder(UCodePointUnknown), Symbol::Builder(UCodePointUnknown));
    }
    /* On complex functions, this step can take a significant time.
     * A workaround could be to identify big functions to skip simplification at
     * the cost of possible inaccurate evaluations (such as diff(abs(x),x,0) not
     * being undefined). */
    PoincareHelpers::Simplify(&m_expressionDerivate, context, ExpressionNode::ReductionTarget::SystemForApproximation);
    // simplify might return an uninitialized Expression if interrupted
    if (m_expressionDerivate.isUninitialized()) {
      m_expressionDerivate = Poincare::Derivative::Builder(expressionReduced(record, context).clone(), Symbol::Builder(UCodePointUnknown), Symbol::Builder(UCodePointUnknown));
    }
  }
  return m_expressionDerivate;
}


template Coordinate2D<float> ContinuousFunction::templatedApproximateAtParameter<float>(float, Context *, int) const;
template Coordinate2D<double> ContinuousFunction::templatedApproximateAtParameter<double>(double, Context *, int) const;

template Coordinate2D<float> ContinuousFunction::privateEvaluateXYAtParameter<float>(float, Context *, int) const;
template Coordinate2D<double> ContinuousFunction::privateEvaluateXYAtParameter<double>(double, Context *, int) const;


} // namespace Graph
