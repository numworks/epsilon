#include "continuous_function.h"
#include <apps/exam_mode_configuration.h>
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
#include <poincare/print.h>
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

ContinuousFunction::AreaType ContinuousFunction::areaType() const {
  ExpressionNode::Type eqType = equationType();
  if (eqType == ExpressionNode::Type::Inferior || eqType == ExpressionNode::Type::InferiorEqual) {
    return AreaType::Inferior;
  }
  if (eqType == ExpressionNode::Type::Superior || eqType == ExpressionNode::Type::SuperiorEqual) {
    return AreaType::Superior;
  }
  return AreaType::None;
}

ContinuousFunction::SymbolType ContinuousFunction::symbolType() const {
  PlotType functionPlotType = plotType();
  return functionPlotType >= PlotType::Cartesian ? SymbolType::X : static_cast<SymbolType>(functionPlotType);
}

I18n::Message ContinuousFunction::plotTypeMessage() {
  PlotType type = plotType();
  if (!IsPlotTypeInactive(type) && equationType() != ExpressionNode::Type::Equal) {
    // Whichever the plot type, InequationType describes the function
    return I18n::Message::InequationType;
  }
  const size_t plotTypeIndex = static_cast<size_t>(type);
  assert(plotTypeIndex < k_numberOfPlotTypes);
  constexpr I18n::Message k_categories[k_numberOfPlotTypes] = {
      I18n::Message::PolarType,
      I18n::Message::ParametricType,
      I18n::Message::CartesianType,
      I18n::Message::LineType,
      I18n::Message::HorizontalLineType,
      I18n::Message::VerticalLineType,
      I18n::Message::OtherType, // VerticalLines are displayed as Others
      I18n::Message::CircleType,
      I18n::Message::EllipseType,
      I18n::Message::ParabolaType,
      I18n::Message::HyperbolaType,
      I18n::Message::OtherType,
      I18n::Message::UndefinedType,
      I18n::Message::UnhandledType,
      I18n::Message::Disabled};
  return k_categories[plotTypeIndex];
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

static_assert(static_cast<uint8_t>(ExpressionNode::Type::Equal) + 1 == static_cast<uint8_t>(ExpressionNode::Type::Superior), "equationType() relies on this type order");
static_assert(static_cast<uint8_t>(ExpressionNode::Type::Equal) + 2 == static_cast<uint8_t>(ExpressionNode::Type::Inferior), "equationType() relies on this type order");
static_assert(static_cast<uint8_t>(ExpressionNode::Type::Equal) + 3 == static_cast<uint8_t>(ExpressionNode::Type::SuperiorEqual), "equationType() relies on this type order");
static_assert(static_cast<uint8_t>(ExpressionNode::Type::Equal) + 4 == static_cast<uint8_t>(ExpressionNode::Type::InferiorEqual), "equationType() relies on this type order");

CodePoint ContinuousFunction::equationSymbol() const {
  constexpr static CodePoint k_equationSymbols[] = { '=', '>', '<', UCodePointSuperiorEqual, UCodePointInferiorEqual};
  return k_equationSymbols[static_cast<uint8_t>(equationType()) - static_cast<uint8_t>(ExpressionNode::Type::Equal)];
}

int ContinuousFunction::nameWithArgument(char * buffer, size_t bufferSize) {
  if (isNamed()) {
    return Function::nameWithArgument(buffer, bufferSize);
  }
  return strlcpy(buffer, k_ordinateName, bufferSize);
}

int ContinuousFunction::printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Context * context, bool symbolValue) {
  if (symbolValue) {
    /* With Vertical curves, cursorT != cursorX .
     * We need the value for symbol=... */
    return PoincareHelpers::ConvertFloatToText<double>(isAlongX() ? cursorX : cursorT, buffer, bufferSize, precision);
  }

  PlotType type = plotType();
  if (type == PlotType::Parametric) {
    Preferences::PrintFloatMode mode = Poincare::Preferences::sharedPreferences()->displayMode();
    return Poincare::Print::customPrintf(buffer, bufferSize, "(%*.*ed;%*.*ed)", cursorX, mode, precision, cursorY, mode, precision);
  }
  if (type == PlotType::Polar) {
    return PoincareHelpers::ConvertFloatToText<double>(evaluate2DAtParameter(cursorT, context).x2(), buffer, bufferSize, precision);
  }
  return PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer, bufferSize, precision);
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::setContent(const char * c, Context * context) {
  setCache(nullptr);
  Ion::Storage::Record::ErrorStatus error = ExpressionModelHandle::setContent(c, context);
  if (error == Ion::Storage::Record::ErrorStatus::None && !isNull()) {
    udpateModel(context);
    error = m_model.renameRecordIfNeeded(this, c, context, symbol());
  }
  return error;
}

bool ContinuousFunction::drawDottedCurve() const {
  ExpressionNode::Type eqType = equationType();
  return eqType == ExpressionNode::Type::Superior || eqType == ExpressionNode::Type::Inferior;
}

bool ContinuousFunction::isNamed() const {
  // Unnamed functions have a fullname starting with k_unnamedRecordFirstChar
  const char * recordFullName = fullName();
  return recordFullName != nullptr && recordFullName[0] != k_unnamedRecordFirstChar;
}

void ContinuousFunction::getLineParameters(double * slope, double * intercept, Context * context) const {
  assert(plotType() == PlotType::Line);
  Expression equation = expressionReduced(context);
  // Compute metrics for details view of Line
  Preferences::ComplexFormat complexFormat = Preferences::sharedPreferences()->complexFormat();
  Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
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

Conic ContinuousFunction::getConicParameters(Context * context) const {
  assert(isConic());
  return Conic(expressionEquation(context), context, k_unknownName);
}

void ContinuousFunction::udpateModel(Context * context) {
  bool previousAlongXStatus = isAlongX();
  updatePlotType(context);
  if (previousAlongXStatus != isAlongX() || canHavePlotRestrictions()) {
    // The definition's domain must be resetted.
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

double ContinuousFunction::approximateDerivative(double x, Context * context, int subCurveIndex) const {
  assert(isAlongX());
  if (x < tMin() || x > tMax() || hasVerticalLines()) {
    return NAN;
  }
  // Derivative is simplified once and for all
  Expression derivate = expressionDerivateReduced(context);
  if (numberOfSubCurves() > 1) {
      assert(derivate.type() == ExpressionNode::Type::Dependency);
      derivate = derivate.childAtIndex(0);
      assert(derivate.type() == ExpressionNode::Type::Matrix && derivate.numberOfChildren() > subCurveIndex);
      derivate = derivate.childAtIndex(subCurveIndex);
  } else {
    assert(subCurveIndex == 0);
  }
  return PoincareHelpers::ApproximateWithValueForSymbol(derivate, k_unknownName, x, context);
}

void ContinuousFunction::setTMin(float tMin) {
  recordData()->setTMin(tMin);
  setCache(nullptr);
}

void ContinuousFunction::setTMax(float tMax) {
  recordData()->setTMax(tMax);
  setCache(nullptr);
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
    assert(numberOfSubCurves() == 1);
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

  if (numberOfSubCurves() >= 2) {
    assert(numberOfSubCurves() == 2);
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
    assert(numberOfSubCurves() == 1);
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
  if (yMaxForced - yMinForced <= ratio * (xMax - xMin) && numberOfSubCurves() == 1) {
    Zoom::RangeWithRatioForDisplay(evaluation, ratio, xMin, xMax, yMinForced, yMaxForced, yMin, yMax, context, this);
    // if (numberOfSubCurves() >= 2) {
    //   assert(numberOfSubCurves() == 2);
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

  if (numberOfSubCurves() >= 2) {
    assert(numberOfSubCurves() == 2);
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
  setCache(nullptr);
  // Retrieve ContinuousFunction's equation
  Expression equation = expressionEquation(context);
  if (equation.type() == ExpressionNode::Type::Undefined) {
    return recordData()->setPlotType(PlotType::Undefined);
  }
  // Compute equation's degree regarding y and x.
  int yDeg = equation.polynomialDegree(context, k_ordinateName);
  int xDeg = equation.polynomialDegree(context, k_unknownName);

  // Inequations : equation symbol has been updated when parsing the equation
  recordData()->setEquationType(m_model.equationType());

  // TODO Hugo : Improve how named functions are detected
  // Named functions : PlotType has been updated when parsing the equation
  if (m_model.plotType() != PlotType::Undefined) {
    // There should be no y symbol. Inequations are handled on cartesians only
    if (yDeg > 0 || (m_model.equationType() != ExpressionNode::Type::Equal && m_model.plotType() != PlotType::Cartesian)) {
      return recordData()->setPlotType(PlotType::Unhandled);
    }
    if (ExamModeConfiguration::inequalityGraphingIsForbidden() && m_model.equationType() != ExpressionNode::Type::Equal) {
      return recordData()->setPlotType(PlotType::Disabled);
    }
    // TODO : f(x)=1+x could be labelled as line.
    return recordData()->setPlotType(m_model.plotType());
  }

  bool isYMainSymbol = (yDeg != 0);
  if (yDeg < 0 || yDeg > 2 || (!isYMainSymbol && xDeg != 1 && xDeg != 2)) {
    // Any equation with such a y and x degree won't be handled anyway.
    return recordData()->setPlotType(PlotType::Unhandled);
  }

  const char * symbolName = isYMainSymbol ? k_ordinateName : k_unknownName;
  ExpressionNode::Sign ySign = ExpressionNode::Sign::Unknown;
  if (!hasNonNullCoefficients(equation, symbolName, context, &ySign)) {
    // The equation must have at least one nonNull coefficient.
    return recordData()->setPlotType(PlotType::Unhandled);
  }

  if (m_model.equationType() != ExpressionNode::Type::Equal) {
    if (ySign == ExpressionNode::Sign::Unknown || (yDeg == 2 && xDeg == -1)) {
      /* Are unhandled equation with :
       * - An unknown highest coefficient sign: sign must be strict and constant
       * - A non polynomial x coefficient in a quadratic equation on y. */
      return recordData()->setPlotType(PlotType::Unhandled);
    }
    if (ExamModeConfiguration::inequalityGraphingIsForbidden()) {
      return recordData()->setPlotType(PlotType::Disabled);
    }
    if (ySign == ExpressionNode::Sign::Negative) {
      // Oppose the comparison operator
      ExpressionNode::Type newEquationType = ComparisonOperator::Opposite(m_model.equationType());
      m_model.setEquationType(newEquationType);
      recordData()->setEquationType(newEquationType);
    }
  }

  /* We can now rely on x and y degree to identify plot type :
   * | y  | x  | Status
   * | 0  | 1  | Vertical Line
   * | 0  | 2  | Vertical Lines
   * | 1  | 0  | Horizontal Line
   * | 1  | 1  | Line
   * | 1  | +  | Cartesian
   * | 2  | 0  | Other (Two Horizontal Lines)
   * | 2  | 1  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | 2  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | +  | Other
   *
   * Other cases should have been escaped above.
   */
  if (yDeg == 0) {
    if (xDeg == 1) {
      return recordData()->setPlotType(PlotType::VerticalLine);
    }
    if (xDeg == 2) {
      return recordData()->setPlotType(ExamModeConfiguration::implicitPlotsAreForbidden() ? PlotType::Disabled : PlotType::VerticalLines);
    }
  }

  if (yDeg == 1 && xDeg == 0) {
    return recordData()->setPlotType(PlotType::HorizontalLine);
  }

  if (yDeg == 1 && xDeg == 1 && ySign != ExpressionNode::Sign::Unknown) {
    // An Unknown y coefficient sign might mean it depends on x (y*x = ...)
    return recordData()->setPlotType(PlotType::Line);
  }

  if (ExamModeConfiguration::implicitPlotsAreForbidden()) {
    if (yDeg == 2 || ySign == ExpressionNode::Sign::Unknown) {
      // Equation with y^2 or such as y*x=1 are disabled.
      return recordData()->setPlotType(PlotType::Disabled);
    }
    // Deliberately ignore conics (such as y=x^2) to hide details.
  } else if (yDeg >= 1 && xDeg >= 1 && xDeg <= 2) {
    // Try to identify a conic. For instance, x*y=1 as an hyperbola
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
    // A conic could not be identified.
  }

  if (yDeg == 1) {
    return recordData()->setPlotType(PlotType::Cartesian);
  }

  assert(yDeg == 2);
  // Unknown type that we are able to plot anyway.
  return recordData()->setPlotType(PlotType::Other);
}

bool ContinuousFunction::hasNonNullCoefficients(Expression equation, const char * symbolName, Context * context, ExpressionNode::Sign * highestDegreeCoefficientSign) const {
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  Preferences::ComplexFormat complexFormat = Preferences::sharedPreferences()->complexFormat();
  Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  int degree = equation.getPolynomialReducedCoefficients(symbolName, coefficients, context, complexFormat, angleUnit, k_defaultUnitFormat, k_defaultSymbolicComputation);
  assert(degree >= 0 && degree <= Expression::k_maxPolynomialDegree);
  if (highestDegreeCoefficientSign != nullptr && degree >= 0) {
    ExpressionNode::Sign sign = coefficients[degree].sign(context);
    if (sign == ExpressionNode::Sign::Unknown) {
      // Approximate for a better estimation. Nan if coefficient depends on x/y.
      double approximation = coefficients[degree].approximateToScalar<double>(context, complexFormat, angleUnit);
      if (!std::isnan(approximation) && approximation != 0.0) {
        sign = approximation < 0.0 ? ExpressionNode::Sign::Negative : ExpressionNode::Sign::Positive;
      }
    }
    *highestDegreeCoefficientSign = sign;
  }
  // Look for a NonNull coefficient.
  for (int d = 0; d <= degree; d++) {
    ExpressionNode::NullStatus nullStatus = coefficients[d].nullStatus(context);
    if (nullStatus == ExpressionNode::NullStatus::NonNull) {
      return true;
    }
    if (nullStatus == ExpressionNode::NullStatus::Unknown) {
      // Approximate for a better estimation. Nan if coefficient depends on x/y.
      double approximation = coefficients[d].approximateToScalar<double>(context, complexFormat, angleUnit);
      if (!std::isnan(approximation) && approximation != 0.0) {
        return true;
      }
    }
  }
  return false;
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
Coordinate2D<T> ContinuousFunction::privateEvaluateXYAtParameter(T t, Context * context, int subCurveIndex) const {
  Coordinate2D<T> x1x2 = templatedApproximateAtParameter(t, context, subCurveIndex);
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
Coordinate2D<T> ContinuousFunction::templatedApproximateAtParameter(T t, Context * context, int subCurveIndex) const {
  if (t < tMin() || t > tMax()) {
    return Coordinate2D<T>(isAlongX() ? t : NAN, NAN);
  }
  Expression e = expressionReduced(context);
  PlotType type = plotType();
  if (type != PlotType::Parametric) {
    if (numberOfSubCurves() >= 2) {
      assert(e.numberOfChildren() > subCurveIndex);
      e = e.childAtIndex(subCurveIndex);
    } else {
      assert(subCurveIndex == 0);
    }
    if (type == PlotType::VerticalLine || type == PlotType::VerticalLines) {
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

/* ContinuousFunction::Model */

Expression ContinuousFunction::Model::expressionReduced(const Ion::Storage::Record * record, Context * context) const {
  // m_expression might already be memmoized.
  if (m_expression.isUninitialized()) {
    // Retrieve the expression equation's expression.
    m_expression = expressionEquation(record, context);
    m_numberOfSubCurves = 1;
    if (record->fullName() == nullptr || record->fullName()[0] == k_unnamedRecordFirstChar) {
      /* Function isn't named, m_expression currently is an expression in y or x
       * such as y = x. We extract the solution by solving in y or x. */
      int yDegree = m_expression.polynomialDegree(context, k_ordinateName);
      Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
      if (yDegree < 0 || yDegree > 2) {
        // Such degrees of equation in y are not handled.
        m_expression = Undefined::Builder();
        return m_expression;
      }
      bool isVertical = (yDegree == 0);
      // Solve the equation in y (or x if isVertical)
      Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
      int degree = m_expression.getPolynomialReducedCoefficients(isVertical ? k_unknownName : k_ordinateName, coefficients, context, Preferences::ComplexFormat::Cartesian, angleUnit, k_defaultUnitFormat, k_defaultSymbolicComputation);
      assert(isVertical || degree == yDegree);
      if (degree == 1) {
        Polynomial::LinearPolynomialRoots(coefficients[1], coefficients[0], &m_expression, context, Preferences::ComplexFormat::Real, angleUnit);
      } else if (degree == 2) {
        // Equation is of degree 2, each root is a subcurve to plot.
        Expression root1, root2, delta;
        int solutions = Polynomial::QuadraticPolynomialRoots(coefficients[2], coefficients[1], coefficients[0], &root1, &root2, &delta, context, Preferences::ComplexFormat::Real, angleUnit);
        if (solutions <= 1) {
          m_expression = root1;
        } else {
          m_numberOfSubCurves = 2;
          // SubCurves are stored in a 2x1 matrix
          Matrix newExpr = Matrix::Builder();
          // Roots are ordered so that the first one is superior to the second
          newExpr.addChildAtIndexInPlace(root2, 0, 0);
          newExpr.addChildAtIndexInPlace(root1, 1, 1);
          newExpr.setDimensions(2, 1);
          m_expression = newExpr;
        }
      } else {
        /* TODO : We could handle simple equations of any degree by solving the
         * equation within the graph view bounds, to plot as many vertical or
         * horizontal lines as needed. */
        m_expression = Undefined::Builder();
        return m_expression;
      }
    }
    // Reduce m_expression to optimize approximations
    PoincareHelpers::CloneAndReduce(&m_expression, context, ExpressionNode::ReductionTarget::SystemForApproximation);
  }
  return m_expression;
}

Expression ContinuousFunction::Model::expressionClone(const Ion::Storage::Record * record) const {
  assert(record->fullName() != nullptr && record->fullName()[0] != k_unnamedRecordFirstChar);
  Expression e = ExpressionModel::expressionClone(record);
  assert(ComparisonOperator::IsComparisonOperatorType(e.type()));
  return e.childAtIndex(1);
}

Expression ContinuousFunction::Model::originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const {
  Expression unknownSymbolEquation = ExpressionModel::expressionClone(record);
  if (unknownSymbolEquation.isUninitialized()) {
    return unknownSymbolEquation;
  }
  return unknownSymbolEquation.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Symbol::Builder(symbol));
}

bool isValidNamedLeftExpression(const Expression e, ExpressionNode::Type equationType) {
  /* Examples of valid named expression : f(x)= or f(x)< or f(θ)= or f(t)=
   * Examples of invalid named expression : cos(x)= or f(θ)< or f(t)<  */
  if (e.type() != ExpressionNode::Type::Function) {
    return false;
  }
  Expression functionSymbol = e.childAtIndex(0);
  return (functionSymbol.isIdenticalTo(Symbol::Builder('x'))
          || (equationType == ExpressionNode::Type::Equal
              && (functionSymbol.isIdenticalTo(
                      Symbol::Builder(UCodePointGreekSmallLetterTheta))
                  || functionSymbol.isIdenticalTo(Symbol::Builder('t')))));
}

Expression ContinuousFunction::Model::expressionEquation(const Ion::Storage::Record * record, Context * context) const {
  // See comment on isCircularlyDefined in ExpressionModel::expressionReduced.
  // Initialize plot type (which might be decided in this method)
  m_plotType = PlotType::Undefined;
  if (record->fullName() != nullptr && record->fullName()[0] != k_unnamedRecordFirstChar && isCircularlyDefined(record, context)) {
    return Undefined::Builder();
  }
  Expression result = ExpressionModel::expressionClone(record);
  if (result.isUninitialized()) {
    return Undefined::Builder();
  }
  m_equationType = result.type();
  assert(ComparisonOperator::IsComparisonOperatorType(m_equationType));
  bool isUnnamedFunction = true;
  Expression leftExpression = result.childAtIndex(0);

  if (isValidNamedLeftExpression(leftExpression, m_equationType)) {
    // Ensure that function name is either record's name, or free
    assert(record->fullName() != nullptr);
    const char * functionName = static_cast<Poincare::Function&>(leftExpression).name();
    const size_t functionNameLength = strlen(functionName);
    if (Shared::GlobalContext::SymbolAbstractNameIsFree(functionName)
        || (record->fullName()[0] != k_unnamedRecordFirstChar
            && memcmp(record->fullName(), functionName, functionNameLength) == 0
            && record->fullName()[functionNameLength] == Ion::Storage::k_dotChar)) {
      Expression functionSymbol = leftExpression.childAtIndex(0);
      // Set the model's plot type.
      if (functionSymbol.isIdenticalTo(Symbol::Builder('t'))) {
        m_plotType = PlotType::Parametric;
      } else if (functionSymbol.isIdenticalTo(Symbol::Builder('x'))) {
        m_plotType = PlotType::Cartesian;
      } else {
        m_plotType = PlotType::Polar;
      }
      result = result.childAtIndex(1);
      isUnnamedFunction = false;
    } else {
      /* Function in first half of the equation refer to an already defined one.
       * Replace the symbol. */
      leftExpression.replaceChildAtIndexInPlace(0, Symbol::Builder(UCodePointUnknown));
    }
  }
  if (isUnnamedFunction) {
    result = Subtraction::Builder(leftExpression, result.childAtIndex(1));
  }
  PoincareHelpers::CloneAndReduce(&result, context, ExpressionNode::ReductionTarget::SystemForAnalysis);
  return result;
}

Expression ContinuousFunction::Model::expressionDerivateReduced(const Ion::Storage::Record * record, Context * context) const {
  // m_expressionDerivate might already be memmoized.
  if (m_expressionDerivate.isUninitialized()) {
    Expression expression = expressionReduced(record, context).clone();
    if (numberOfSubCurves() > 1) {
      // Derive each curve individually, return a matrix of each derivatives
      Matrix newExpr = Matrix::Builder();
      assert(expression.type() == ExpressionNode::Type::Matrix);
      for (size_t i = 0; i < numberOfSubCurves(); i++) {
        newExpr.addChildAtIndexInPlace(Derivative::Builder(expression.childAtIndex(i), Symbol::Builder(UCodePointUnknown), Symbol::Builder(UCodePointUnknown)), i, i);
      }
      newExpr.setDimensions(numberOfSubCurves(), 1);
      m_expressionDerivate = newExpr;
    } else {
      m_expressionDerivate = Derivative::Builder(expression, Symbol::Builder(UCodePointUnknown), Symbol::Builder(UCodePointUnknown));
    }
    /* On complex functions, this step can take a significant time.
     * A workaround could be to identify big functions to skip simplification at
     * the cost of possible inaccurate evaluations (such as diff(abs(x),x,0) not
     * being undefined). */
    PoincareHelpers::CloneAndSimplify(&m_expressionDerivate, context, ExpressionNode::ReductionTarget::SystemForApproximation);
  }
  return m_expressionDerivate;
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::Model::renameRecordIfNeeded(Ion::Storage::Record * record, const char * c, Context * context, CodePoint symbol) {
  Expression newExpression = originalEquation(record, symbol);
  Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
  if (Ion::Storage::FullNameHasExtension(record->fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension))) {
    if (!newExpression.isUninitialized()
        && ComparisonOperator::IsComparisonOperatorType(newExpression.type())
        && isValidNamedLeftExpression(newExpression.childAtIndex(0),
                                      newExpression.type())) {
      Expression function = newExpression.childAtIndex(0);
      error = Ion::Storage::Record::SetBaseNameWithExtension(record, static_cast<SymbolAbstract&>(function).name(), Ion::Storage::funcExtension);
      if (error != Ion::Storage::Record::ErrorStatus::NameTaken) {
        return error;
      }
      // Function's name is already taken, reset records name if needed.
      error = Ion::Storage::Record::ErrorStatus::None;
    }
    if (record->fullName()[0] == k_unnamedRecordFirstChar) {
      // Record is already unnamed (and hidden).
      return error;
    }
    // Rename record with a hidden record name.
    char name[k_maxDefaultNameSize];
    const char * const extensions[1] = { Ion::Storage::funcExtension };
    name[0] = k_unnamedRecordFirstChar;
    Ion::Storage::sharedStorage()->firstAvailableNameFromPrefix(name, 1, k_maxDefaultNameSize, extensions, 1, 99);
    error = Ion::Storage::Record::SetBaseNameWithExtension(record, name, Ion::Storage::funcExtension);
  }
  return error;
}

Poincare::Expression ContinuousFunction::Model::buildExpressionFromText(const char * c, CodePoint symbol, Poincare::Context * context) const {
  Expression noContextExpression;
  // if c = "", we want to reinit the Expression
  if (c && *c != 0) {
    /* Compute the expression to store, without replacing symbols. No context is
     * given so that any unknown function name is parsed as function. */
    noContextExpression = Expression::Parse(c, nullptr);
    if (noContextExpression.isUninitialized()) {
      return noContextExpression;
    }
    // Check if the equation is of the form f(x)=...
    ExpressionNode::Type comparisonType = noContextExpression.type();
    if (ComparisonOperator::IsComparisonOperatorType(comparisonType) && isValidNamedLeftExpression(noContextExpression.childAtIndex(0), comparisonType)) {
      Expression functionSymbol = noContextExpression.childAtIndex(0).childAtIndex(0);
      // Extract the CodePoint function's symbol. We know it is either x, t or θ
      assert(functionSymbol.type() == ExpressionNode::Type::Symbol);
      // Override the symbol so that it can be replaced in the right expression
      if (functionSymbol.isIdenticalTo(Symbol::Builder('x'))) {
        symbol = 'x';
      } else if (functionSymbol.isIdenticalTo(Symbol::Builder('t'))) {
        symbol = 't';
      } else {
        assert((functionSymbol.isIdenticalTo(Symbol::Builder(UCodePointGreekSmallLetterTheta))));
        symbol = UCodePointGreekSmallLetterTheta;
      }
    } else {
      // Fall back on default ExpressionModel::buildExpressionFromText behavior
      noContextExpression = Expression();
    }
  }
  Expression expressionToStore = ExpressionModel::buildExpressionFromText(c, symbol, context);
  if (!noContextExpression.isUninitialized() && !expressionToStore.isUninitialized()) {
    assert(Poincare::ComparisonOperator::IsComparisonOperatorType(expressionToStore.type()));
    // Preserve the new function and its symbol
    expressionToStore.replaceChildAtIndexInPlace(0, noContextExpression.childAtIndex(0));
  }
  return expressionToStore;
}

void ContinuousFunction::Model::tidyDownstreamPoolFrom(char * treePoolCursor) const {
  if (treePoolCursor == nullptr || m_expressionDerivate.isDownstreamOf(treePoolCursor)) {
    m_numberOfSubCurves = 1;
    m_equationType = ExpressionNode::Type::Equal;
    m_plotType = PlotType::Undefined;
    m_expressionDerivate = Expression();
  }
  ExpressionModel::tidyDownstreamPoolFrom(treePoolCursor);
}

void * ContinuousFunction::Model::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer+sizeof(RecordDataBuffer);
}

size_t ContinuousFunction::Model::expressionSize(const Ion::Storage::Record * record) const {
  return record->value().size-sizeof(RecordDataBuffer);
}


template Coordinate2D<float> ContinuousFunction::templatedApproximateAtParameter<float>(float, Context *, int) const;
template Coordinate2D<double> ContinuousFunction::templatedApproximateAtParameter<double>(double, Context *, int) const;

template Coordinate2D<float> ContinuousFunction::privateEvaluateXYAtParameter<float>(float, Context *, int) const;
template Coordinate2D<double> ContinuousFunction::privateEvaluateXYAtParameter<double>(double, Context *, int) const;


} // namespace Graph
