#include "continuous_function.h"
#include <apps/exam_mode_configuration.h>
#include <apps/apps_container_helper.h>
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
#include <poincare/derivative.h>
#include <poincare/print.h>
#include "poincare_helpers.h"
#include <algorithm>
#include "global_context.h"

using namespace Poincare;

namespace Shared {

constexpr char ContinuousFunction::k_ordinateName[2];
constexpr CodePoint ContinuousFunction::k_cartesianSymbol;
constexpr CodePoint ContinuousFunction::k_parametricSymbol;
constexpr CodePoint ContinuousFunction::k_polarSymbol;
constexpr CodePoint ContinuousFunction::k_ordinateCodePoint;
constexpr CodePoint ContinuousFunction::k_unnamedExpressionSymbol;

/* ContinuousFunction - Public */

ContinuousFunction ContinuousFunction::NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName) {
  static int s_colorIndex = 0;
  assert(baseName != nullptr);
  // Create the record
  RecordDataBuffer data(Escher::Palette::nextDataColor(&s_colorIndex));
  *error = Ion::Storage::FileSystem::sharedFileSystem()->createRecordWithExtension(baseName, Ion::Storage::funcExtension, &data, sizeof(data));
  if (*error != Ion::Storage::Record::ErrorStatus::None) {
    return ContinuousFunction();
  }
  // Return the ContinuousFunction with the new record
  return ContinuousFunction(Ion::Storage::FileSystem::sharedFileSystem()->recordBaseNamedWithExtension(baseName, Ion::Storage::funcExtension));
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
  ComparisonNode::OperatorType eqType = equationType();
  PlotType type = plotType();
  if (IsPlotTypeInactive(type) || eqType == ComparisonNode::OperatorType::Equal) {
    return AreaType::None;
  }
  assert(type <= PlotType::Other);
  // To draw y^2>a, the area plotted should be Outside and not Above.
  bool inequationIsLinear = type <= PlotType::VerticalLine;
  assert(numberOfSubCurves() == 1 || !inequationIsLinear);
  if (eqType == ComparisonNode::OperatorType::Inferior || eqType == ComparisonNode::OperatorType::InferiorEqual) {
    return inequationIsLinear ? AreaType::Below : AreaType::Inside;
  }
  assert(eqType == ComparisonNode::OperatorType::Superior || eqType == ComparisonNode::OperatorType::SuperiorEqual);
  return inequationIsLinear ? AreaType::Above : AreaType::Outside;
}

ContinuousFunction::PlotType ContinuousFunction::plotType() const {
  if (m_model.plotType() == PlotType::Unknown) {
    // Computing the expression equation will update the unknown plot type.
    expressionReducedForAnalysis(AppsContainerHelper::sharedAppsContainerGlobalContext());
  }
  assert(m_model.plotType() != PlotType::Unknown);
  return m_model.plotType();
}

ContinuousFunction::SymbolType ContinuousFunction::symbolType() const {
  PlotType functionPlotType = plotType();
  switch (functionPlotType) {
  case PlotType::Parametric:
  case PlotType::UndefinedParametric:
  case PlotType::UnhandledParametric:
    return SymbolType::T;
  case PlotType::Polar:
  case PlotType::UndefinedPolar:
  case PlotType::UnhandledPolar:
    return SymbolType::Theta;
  default:
    return SymbolType::X;
  }
}

I18n::Message ContinuousFunction::plotTypeMessage() const {
  PlotType type = plotType();
  if (!IsPlotTypeInactive(type) && equationType() != ComparisonNode::OperatorType::Equal) {
    // Whichever the plot type, InequationType describes the function
    return I18n::Message::InequationType;
  }
  const uint8_t plotTypeIndex = static_cast<uint8_t>(type);
  assert(plotTypeIndex < static_cast<uint8_t>(PlotType::NumberOfPlotTypes));
  constexpr I18n::Message k_categories[static_cast<uint8_t>(PlotType::NumberOfPlotTypes)] = {
      I18n::Message::CartesianType,
      I18n::Message::ParabolaType,  // CartesianParabola displayed as Parabola
      I18n::Message::HyperbolaType, // CartesianHyperbola displayed as Hyperbola
      I18n::Message::LineType,
      I18n::Message::HorizontalLineType,
      I18n::Message::InverseType,
      I18n::Message::VerticalLineType,
      I18n::Message::OtherType,     // OtherAlongY displayed as Others
      I18n::Message::CircleType,
      I18n::Message::EllipseType,
      I18n::Message::ParabolaType,
      I18n::Message::HyperbolaType,
      I18n::Message::OtherType,
      I18n::Message::PolarType,
      I18n::Message::ParametricType,
      I18n::Message::UndefinedType,
      I18n::Message::UndefinedType, // UndefinedPolar displayed as Undefined
      I18n::Message::UndefinedType, // UndefinedParametric displayed as Undefined
      I18n::Message::UnhandledType,
      I18n::Message::UnhandledType, // UnhandledPolar displayed as Unhandled
      I18n::Message::UnhandledType, // UnhandledParametric displayed as Unhandled
      I18n::Message::Disabled,
      I18n::Message::UndefinedType};      // Shouldn't be displayed
  return k_categories[plotTypeIndex];
}

CodePoint ContinuousFunction::symbol() const {
  switch (symbolType()) {
  case SymbolType::T:
    return k_parametricSymbol;
  case SymbolType::Theta:
    return k_polarSymbol;
  default:
    return k_cartesianSymbol;
  }
}

CodePoint ContinuousFunction::equationSymbol() const {
  return ComparisonNode::ComparisonCodePoint(equationType());
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
    return PoincareHelpers::ConvertFloatToText<double>(isAlongXOrY() ? cursorX : cursorT, buffer, bufferSize, precision);
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
  bool wasAlongXorY = isAlongXOrY();
  /* About to set the content, the symbol does not matter here yet. We don't use
   * ExpressionModelHandle::setContent implementation to avoid calling symbol()
   * and any unnecessary plot type update at this point. See comment in
   * ContinuousFunction::Model::buildExpressionFromText. */
  Ion::Storage::Record::ErrorStatus error = editableModel()->setContent(this, c, context, k_unnamedExpressionSymbol);
  if (error == Ion::Storage::Record::ErrorStatus::None && !isNull()) {
    updateModel(context, wasAlongXorY);
    error = m_model.renameRecordIfNeeded(this, c, context, symbol());
  }
  return error;
}

void ContinuousFunction::tidyDownstreamPoolFrom(char * treePoolCursor) const {
  ExpressionModelHandle::tidyDownstreamPoolFrom(treePoolCursor);
  m_cache = nullptr;
}

bool ContinuousFunction::drawDottedCurve() const {
  ComparisonNode::OperatorType eqType = equationType();
  return eqType == ComparisonNode::OperatorType::Superior || eqType == ComparisonNode::OperatorType::Inferior;
}

bool ContinuousFunction::isActiveInTable() const {
  /* In addition to isActive(), a function must not be an inequality, must not
   * have any vertical lines and must always plot with a single subcurve. */
  static_assert(PlotType::CartesianAlongY > PlotType::HorizontalLine, "CartesianAlongY shouldn't be active in table.");
  return equationType() == ComparisonNode::OperatorType::Equal
         && (plotType() <= PlotType::HorizontalLine
             || plotType() == PlotType::Polar
             || plotType() == PlotType::Parametric)
         && isActive();
}

bool ContinuousFunction::isConic() const {
  switch (plotType()) {
    case PlotType::CartesianParabola:
    case PlotType::CartesianHyperbola:
    case PlotType::Circle:
    case PlotType::Ellipse:
    case PlotType::Parabola:
    case PlotType::Hyperbola:
      return true;
    default:
      return false;
  }
}

bool ContinuousFunction::isNamed() const {
  // Unnamed functions have a fullname starting with k_unnamedRecordFirstChar
  const char * recordFullName = fullName();
  return recordFullName != nullptr && recordFullName[0] != k_unnamedRecordFirstChar;
}

bool ContinuousFunction::isDiscontinuousBetweenFloatValues(float x1, float x2, Poincare::Context * context) const {
  Expression equation = expressionReduced(context);
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  return equation.isDiscontinuousBetweenValuesForSymbol(k_unknownName, x1, x2, context, preferences->complexFormat(), preferences->angleUnit());
}

void ContinuousFunction::getLineParameters(double * slope, double * intercept, Context * context) const {
  assert(plotType() == PlotType::Line);
  Expression equation = expressionReduced(context);
  // Compute metrics for details view of Line
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Separate the two line coefficients for approximation.
  int d = equation.getPolynomialReducedCoefficients(
      k_unknownName, coefficients, context, ComplexFormat(), AngleUnit(),
      k_defaultUnitFormat,
      ExpressionNode::SymbolicComputation::
          ReplaceAllSymbolsWithDefinitionsOrUndefined);
  assert(d <= 1);
  // Degree might vary depending on symbols definition and complex format.
  // Approximate and return the two line coefficients
  if (d < 0) {
    *slope = NAN;
    *intercept = NAN;
  } else {
    *intercept = coefficients[0].approximateToScalar<double>(context, ComplexFormat(), AngleUnit());
    if (d == 0) {
      *slope = 0.0;
    } else {
      *slope = coefficients[1].approximateToScalar<double>(context, ComplexFormat(), AngleUnit());
    }
  }
}

Conic ContinuousFunction::getConicParameters(Context * context) const {
  assert(isConic());
  return Conic(expressionReducedForAnalysis(context), context, k_unknownName);
}

int ContinuousFunction::numberOfCurveParameters() const {
  return plotType() == PlotType::Parametric ? 3 : 2;
}

ContinuousFunction::CurveParameter ContinuousFunction::getCurveParameter(int index) const {
  using namespace I18n;
  switch (plotType()) {
  case PlotType::Cartesian:
    return {index == 0 ? Message::X : Message::Default, true, .isPreimage = index == 1};
  case PlotType::Line:
    return {index == 0 ? Message::X : Message::Y, true, .isPreimage = index == 1};
  case PlotType::HorizontalLine:
    return {index == 0 ? Message::X : Message::Y, index == 0};
  case PlotType::VerticalLine:
    return {index == 0 ? Message::X : Message::Y, index == 1};
  case PlotType::Parametric:
    return {index == 0 ? Message::T : index == 1 ? Message::XOfT : Message::YOfT, index == 0};
  case PlotType::Polar:
    return {index == 0 ? Message::Theta : Message::R, index == 0};
  default:
    // Conics
    assert(plotType() < PlotType::Undefined);
    return {index == 0 ? Message::X : Message::Y, false};
  }
}

double ContinuousFunction::evaluateCurveParameter(int index, double cursorT, double cursorX, double cursorY, Context * context) const {
  switch (plotType()) {
  case PlotType::Parametric:
    return index == 0 ? cursorT : index == 1 ? evaluateXYAtParameter(cursorT, context).x1() : evaluateXYAtParameter(cursorT, context).x2();
  case PlotType::Polar:
    return index == 0 ? cursorT : evaluate2DAtParameter(cursorT, context).x2();
  default:
    // Expressions of x and y
    assert(plotType() < PlotType::Polar);
    // The subcurve number would need to be passed down here to properly assert
    assert(numberOfSubCurves() > 1 || evaluateXYAtParameter(cursorT, context).x1() == cursorX && evaluateXYAtParameter(cursorT, context).x2() == cursorY);
    return index == 0 ? cursorX : cursorY;
  }
}

void ContinuousFunction::updateModel(Context * context, bool wasAlongXorY) {
  setCache(nullptr);
  // Reset model's plot type. expressionReducedForAnalysis() will update plotType
  m_model.resetPlotType();
  expressionReducedForAnalysis(context);
  assert(m_model.plotType() != PlotType::Unknown);
  if (wasAlongXorY != isAlongXOrY() || !canHaveCustomDomain()) {
    // The definition's domain must be reset.
    setTMin(!isAlongXOrY() ? 0.0 : -INFINITY);
    setTMax(!isAlongXOrY() ? 2.0 * Trigonometry::PiInAngleUnit(AngleUnit())
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
  assert(canDisplayDerivative());
  if (x < tMin() || x > tMax() || isAlongY() || numberOfSubCurves() > 1) {
    return NAN;
  }
  // Derivative is simplified once and for all
  Expression derivate = expressionDerivateReduced(context);
  assert(subCurveIndex == 0);
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
  if (!isAlongXOrY()) {
    assert(std::isfinite(tMin()) && std::isfinite(tMax()) && std::isfinite(rangeStep()) && rangeStep() > 0);
    protectedFullRangeForDisplay(tMin(), tMax(), rangeStep(), xMin, xMax, context, true);
    assert(numberOfSubCurves() == 1);
    *yMinIntrinsic = FLT_MAX;
    *yMaxIntrinsic = -FLT_MAX;
    return;
  }

  /* Call basedOnCostlyAlgorithms first to ensure expressionReduced is called
   * and the conic equation is solved if applicable. */
  if (basedOnCostlyAlgorithms(context)) {
    /* The function makes use of some costly algorithms, such as integration or
     * sequences, and cannot be computed in a timely manner. */
    return;
  }

  if (isAlongY()) {
    /* Vertical lines' x range are quite simple and could not be computed with
     * InterestingRangesForDisplay. We use curve(s) x-position(s). */
    *xMin = FLT_MAX;
    *xMax = -FLT_MAX;
    for (int i = 0; i < numberOfSubCurves(); i++) {
      float line = evaluateXYAtParameter(0.0f, context, i).x1();
      *xMax = std::max<double>(*xMax, line);
      *xMin = std::min<double>(*xMin, line);
    }
    Zoom::AddBreathingRoom(xMin, xMax);
    // No intrinsic y ranges to suggest
    *yMinIntrinsic = FLT_MAX;
    *yMaxIntrinsic = -FLT_MAX;
    return;
  }

  *xMin = NAN;
  *xMax = NAN;
  *yMinIntrinsic = NAN;
  *yMaxIntrinsic = NAN;

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
  if (!isAlongXOrY()) {
    assert(std::isfinite(tMin()) && std::isfinite(tMax()) && std::isfinite(rangeStep()) && rangeStep() > 0);
    protectedFullRangeForDisplay(tMin(), tMax(), rangeStep(), yMin, yMax, context, false);
    assert(numberOfSubCurves() == 1);
    return;
  }

  *yMin = NAN;
  *yMax = NAN;

  if (basedOnCostlyAlgorithms(context) || isAlongY()) {
    /* The function makes use of some costly algorithms, such as integration or
     * sequences, and cannot be computed in a timely manner.
     * Vertical Lines have no interesting y ranges. */
    return;
  }

  if (!optimizeRange) {
    // TODO: Adapt protectedFullRangeForDisplay for functions with two curves
    protectedFullRangeForDisplay(xMin, xMax, (xMax - xMin) / (Ion::Display::Width / 4), yMin, yMax, context, false);
    return;
  }

  Zoom::ValueAtAbscissa evaluation = [](float x, Context * context, const void * auxiliary) {
    return static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context, 0).x2();
  };

  /* TODO: yRangeForDisplay currently doesn't support ContinuousFunctions
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
  assert(isAlongXOrY());
  double tmin = std::max<double>(tMin(), eDomainMin), tmax = std::min<double>(tMax(), eDomainMax);
  start = start < tmin ? tmin : start > tmax ? tmax : start;
  max = max < tmin ? tmin : max > tmax ? tmax : max;
  if (start == max) {
    return NAN;
  }
  return PoincareHelpers::NextIntersection(expressionReduced(context), k_unknownName, start, max, context, e, relativePrecision, minimalStep, maximalStep);
}

Expression ContinuousFunction::sumBetweenBounds(double start, double end, Context * context) const {
  assert(isAlongXOrY());
  start = std::max<double>(start, tMin());
  end = std::min<double>(end, tMax());
  return Integral::Builder(expressionReduced(context).clone(), Symbol::Builder(UCodePointUnknown), Float<double>::Builder(start), Float<double>::Builder(end)); // Integral takes ownership of args
  /* TODO: when we approximate integral, we might want to simplify the integral
   * here. However, we might want to do it once for all x (to avoid lagging in
   * the derivative table. */
}

/* ContinuousFunction - Private */

float ContinuousFunction::rangeStep() const {
  return isAlongXOrY() ? NAN : (tMax() - tMin())/k_polarParamRangeSearchNumberOfPoints;
}

Coordinate2D<double> ContinuousFunction::nextPointOfInterestFrom(double start, double max, Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const {
  assert(isAlongXOrY());
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
  const T angle = x1x2.x1() * M_PI / Trigonometry::PiInAngleUnit(AngleUnit());
  return Coordinate2D<T>(x1x2.x2() * std::cos(angle),
                         x1x2.x2() * std::sin(angle));
}

template<typename T>
Coordinate2D<T> ContinuousFunction::templatedApproximateAtParameter(T t, Context * context, int subCurveIndex) const {
  if (t < tMin() || t > tMax()) {
    return Coordinate2D<T>(isAlongXOrY() ? t : NAN, NAN);
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
    if (isAlongY()) {
      // Invert x and y with vertical lines so it can be scrolled vertically
      return Coordinate2D<T>(PoincareHelpers::ApproximateWithValueForSymbol(e, k_unknownName, t, context), t);
    }
    return Coordinate2D<T>(t, PoincareHelpers::ApproximateWithValueForSymbol(e, k_unknownName, t, context));
  }
  if (e.type() == ExpressionNode::Type::Dependency) {
    e = e.childAtIndex(0);
  }
  if (e.isUndefined()) {
    return Coordinate2D<T>(NAN, NAN);
  }
  // TODO : This should maybe be a List instead of a Matrix
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
    m_expression = expressionReducedForAnalysis(record, context);
    m_numberOfSubCurves = 1;
    if (record->fullName() == nullptr || record->fullName()[0] == k_unnamedRecordFirstChar) {
      /* Function isn't named, m_expression currently is an expression in y or x
       * such as y = x. We extract the solution by solving in y or x. */
      int yDegree = m_expression.polynomialDegree(context, k_ordinateName);
      bool willBeAlongX = true;
      if (yDegree < 1 || yDegree > 2) {
        int xDegree = m_expression.polynomialDegree(context, k_unknownName);
        if (xDegree < 1 || xDegree > 2) {
          // Such degrees of equation in y and x are not handled.
          m_expression = Undefined::Builder();
          return m_expression;
        }
        // Equation can be plotted along y. For example : x=cos(y) or x^2=1
        willBeAlongX = false;
      }
      /* Solve the equation in y (or x if not willBeAlongX)
       * Symbols are replaced to simplify roots. */
      Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
      int degree = m_expression.getPolynomialReducedCoefficients(
          willBeAlongX ? k_ordinateName : k_unknownName, coefficients, context,
          ComplexFormat(), AngleUnit(), k_defaultUnitFormat,
          ExpressionNode::SymbolicComputation::
              ReplaceAllDefinedSymbolsWithDefinition);
      assert(!willBeAlongX || degree == yDegree);
      if (degree == 1) {
        Polynomial::LinearPolynomialRoots(
          coefficients[1],
          coefficients[0],
          &m_expression,
          context,
          ComplexFormat(),
          AngleUnit(),
          false);
      } else if (degree == 2) {
        // Equation is of degree 2, each root is a subcurve to plot.
        Expression root1, root2, delta;
        int solutions = Polynomial::QuadraticPolynomialRoots(
            coefficients[2],
            coefficients[1],
            coefficients[0],
            &root1,
            &root2,
            &delta,
            context,
            ComplexFormat(),
            AngleUnit(),
            false,
            false);
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
      if (!willBeAlongX && yDegree != 0) {
        // No need to replace anything if yDegree is 0
        m_expression.replaceSymbolWithExpression(Symbol::Builder(k_ordinateCodePoint), Symbol::Builder(UCodePointUnknown));
      }
    } else {
      /* m_expression is resulting of a simplification with the target
      * SystemForAnalysis. But SystemForApproximation can be
      * sometimes way simpler than the one from SystemForAnalysis.
      * For example (x+9)^6 is fully developped in SystemForAnalysis,
      * which results in approximation inaccuracy.
      * On the other hand, the expression (x+1)^2-x^2-2x-1 should be developped
      * so that we understand that it's equal to zero, and is better handled
      * by SystemForAnalysis.
      * To solve this problem, we try to simplify both ways and compare the number
      * of nodes of each expression. We take the one that has the less node.
      * This is not ideal because an expression with less node does not always
      * mean a simpler expression, but it's a good compromise for now. */
      Expression resultForApproximation = expressionEquation(record, context, nullptr);
      if (!resultForApproximation.isUninitialized()) {
        PoincareHelpers::CloneAndReduce(
            &resultForApproximation,
            context,
            ExpressionNode::ReductionTarget::SystemForApproximation,
            ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol);
        if (resultForApproximation.numberOfDescendants(true) < m_expression.numberOfDescendants(true)) {
          m_expression = resultForApproximation;
        }
      }
    }
  }
  return m_expression;
}

Poincare::Expression ContinuousFunction::Model::expressionReducedForAnalysis(const Ion::Storage::Record * record, Poincare::Context * context) const {
  PlotType computedPlotType;
  Expression result = expressionEquation(record, context, &computedPlotType);
  if (!result.isUndefined()) {
    PoincareHelpers::CloneAndReduce(
        &result,
        context,
        ExpressionNode::ReductionTarget::SystemForAnalysis,
        ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol); // Symbols have already been replaced.
  } else {
    computedPlotType = PlotType::Undefined;
  }
  if (plotType() == PlotType::Unknown) {
    m_plotType = computedPlotType;
    // Use the computed equation to update the plot type.
    updatePlotType(record, result, context);
  }
  return result;
}


Expression ContinuousFunction::Model::expressionClone(const Ion::Storage::Record * record) const {
  assert(record->fullName() != nullptr && record->fullName()[0] != k_unnamedRecordFirstChar);
  Expression e = ExpressionModel::expressionClone(record);
  if (e.isUninitialized()) {
    return e;
  }
  return e.childAtIndex(1);
}

Expression ContinuousFunction::Model::originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const {
  Expression unknownSymbolEquation = ExpressionModel::expressionClone(record);
  if (unknownSymbolEquation.isUninitialized() || symbol == UCodePointUnknown) {
    return unknownSymbolEquation;
  }
  return unknownSymbolEquation.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Symbol::Builder(symbol));
}

bool isValidNamedLeftExpression(const Expression e, ComparisonNode::OperatorType equationType) {
  /* Examples of valid named expression : f(x)= or f(x)< or f(θ)= or f(t)=
   * Examples of invalid named expression : cos(x)= or f(θ)< or f(t)<  */
  if (e.type() != ExpressionNode::Type::Function || equationType == ComparisonNode::OperatorType::NotEqual) {
    return false;
  }
  Expression functionSymbol = e.childAtIndex(0);
  return functionSymbol.isIdenticalTo(Symbol::Builder(ContinuousFunction::k_cartesianSymbol))
         || (equationType == ComparisonNode::OperatorType::Equal
             && (functionSymbol.isIdenticalTo(Symbol::Builder(ContinuousFunction::k_polarSymbol))
                 || functionSymbol.isIdenticalTo(Symbol::Builder(ContinuousFunction::k_parametricSymbol))));
}

Expression ContinuousFunction::Model::expressionEquation(const Ion::Storage::Record * record, Context * context, PlotType * computedPlotType) const {
  Expression result = ExpressionModel::expressionClone(record);
  if (result.isUninitialized()) {
    return Undefined::Builder();
  }
  PlotType tempPlotType = PlotType::Unknown;
  ComparisonNode::OperatorType equationType;
  if (!ComparisonNode::IsBinaryComparison(result, &equationType) || equationType == ComparisonNode::OperatorType::NotEqual) {
    /* Happens when the inputted text is too long and "f(x)=" can't be inserted
     * or when inputting amiguous equations like "x+y>2>y" */
    return Undefined::Builder();
  }
 if (plotType() == PlotType::Unknown) {
    m_equationType = equationType;
  }
  bool isUnnamedFunction = true;
  Expression leftExpression = result.childAtIndex(0);

  if (isValidNamedLeftExpression(leftExpression, equationType)) {
    // Ensure that function name is either record's name, or free
    assert(record->fullName() != nullptr);
    const char * functionName = static_cast<Poincare::Function&>(leftExpression).name();
    const size_t functionNameLength = strlen(functionName);
    if (Shared::GlobalContext::SymbolAbstractNameIsFree(functionName)
        || (record->fullName()[0] != k_unnamedRecordFirstChar
            && strncmp(record->name().baseName, functionName, functionNameLength) == 0)) {
      Expression functionSymbol = leftExpression.childAtIndex(0);
      // Set the model's plot type.
      if (functionSymbol.isIdenticalTo(Symbol::Builder(k_parametricSymbol))) {
        tempPlotType = PlotType::Parametric;
      } else if (functionSymbol.isIdenticalTo(Symbol::Builder(k_cartesianSymbol))) {
        tempPlotType = PlotType::Cartesian;
      } else {
        assert((functionSymbol.isIdenticalTo(Symbol::Builder(k_polarSymbol))));
        tempPlotType = PlotType::Polar;
      }
      result = result.childAtIndex(1);
      isUnnamedFunction = false;
    } else {
      /* Function in first half of the equation refer to an already defined one.
       * Replace the symbol. */
      leftExpression.replaceChildAtIndexInPlace(0, Symbol::Builder(UCodePointUnknown));
    }
  }
  if (computedPlotType) {
    *computedPlotType = tempPlotType;
  }
  if (isUnnamedFunction) {
    result = Subtraction::Builder(leftExpression, result.childAtIndex(1));
    /* Replace all y symbols with UCodePointTemporaryUnknown so that they are
     * not replaced if they had a predefined value. This will not replace the y
     * symbols nested in function, which is not a supported behavior anyway.
     * TODO: Make a consistent behavior calculation/additional_outputs using a
     *       VariableContext to temporary disable y's predefinition. */
    result = result.replaceSymbolWithExpression(Symbol::Builder(k_ordinateCodePoint), Symbol::Builder(UCodePointTemporaryUnknown));
  }
  // Replace all defined symbols and functions to extract symbols
  result = Expression::ExpressionWithoutSymbols(result, context);
  if (result.isUninitialized()) {
    // result was Circularly defined
    return Undefined::Builder();
  }
  if (isUnnamedFunction) {
    result = result.replaceSymbolWithExpression(Symbol::Builder(UCodePointTemporaryUnknown), Symbol::Builder(k_ordinateCodePoint));
  }
  assert(!result.isUninitialized());
  return result;
}

Expression ContinuousFunction::Model::expressionDerivateReduced(const Ion::Storage::Record * record, Context * context) const {
  // m_expressionDerivate might already be memmoized.
  if (m_expressionDerivate.isUninitialized()) {
    Expression expression = expressionReduced(record, context).clone();
    // Derivative isn't available on curves with multiple subcurves
    if (numberOfSubCurves() > 1) {
      m_expressionDerivate = Undefined::Builder();
    } else {
      m_expressionDerivate = Derivative::Builder(expression, Symbol::Builder(UCodePointUnknown), Symbol::Builder(UCodePointUnknown));
      /* On complex functions, this step can take a significant time.
      * A workaround could be to identify big functions to skip simplification
      * at the cost of possible inaccurate evaluations (such as diff(abs(x),x,0)
      * not being undefined). */
      PoincareHelpers::CloneAndSimplify(&m_expressionDerivate, context, ExpressionNode::ReductionTarget::SystemForApproximation);
    }
  }
  return m_expressionDerivate;
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::Model::renameRecordIfNeeded(Ion::Storage::Record * record, const char * c, Context * context, CodePoint symbol) {
  Expression newExpression = originalEquation(record, symbol);
  Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
  if (record->hasExtension(Ion::Storage::funcExtension)) {
    ComparisonNode::OperatorType newOperatorType;
    if (!newExpression.isUninitialized()
        && ComparisonNode::IsBinaryComparison(newExpression, &newOperatorType)
        && isValidNamedLeftExpression(newExpression.childAtIndex(0), newOperatorType)) {
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
    Ion::Storage::FileSystem::sharedFileSystem()->firstAvailableNameFromPrefix(name, 1, k_maxDefaultNameSize, extensions, 1, 99);
    error = Ion::Storage::Record::SetBaseNameWithExtension(record, name, Ion::Storage::funcExtension);
  }
  return error;
}

Poincare::Expression ContinuousFunction::Model::buildExpressionFromText(const char * c, CodePoint symbol, Poincare::Context * context) const {
  /* The symbol parameter is discarded in this implementation. Either there is a
   * valid named left expression and the symbol will be extracted, either the
   * symbol should be the default symbol used in unnamed expressions. */
  assert(symbol == k_unnamedExpressionSymbol);
  Expression expressionToStore;
  bool isFunctionAssignment = false;
  // if c = "", we want to reinit the Expression
  if (c && *c != 0) {
    /* Parse the expression to store as possible function assignment. */
    expressionToStore = Expression::Parse(c, context, true, true);
    if (expressionToStore.isUninitialized()) {
      return expressionToStore;
    }
    // Check if the equation is of the form f(x)=...
    ComparisonNode::OperatorType comparisonType;
    if (ComparisonNode::IsBinaryComparison(expressionToStore, &comparisonType)
      && isValidNamedLeftExpression(expressionToStore.childAtIndex(0), comparisonType)) {
      isFunctionAssignment = true;
      Expression functionSymbol = expressionToStore.childAtIndex(0).childAtIndex(0);
      // Extract the CodePoint function's symbol. We know it is either x, t or θ
      assert(functionSymbol.type() == ExpressionNode::Type::Symbol);
      // Override the symbol so that it can be replaced in the right expression
      if (functionSymbol.isIdenticalTo(Symbol::Builder(k_cartesianSymbol))) {
        symbol = k_cartesianSymbol;
      } else if (functionSymbol.isIdenticalTo(Symbol::Builder(k_parametricSymbol))) {
        symbol = k_parametricSymbol;
      } else {
        assert((functionSymbol.isIdenticalTo(Symbol::Builder(k_polarSymbol))));
        symbol = k_polarSymbol;
      }
    }
  }
  if (isFunctionAssignment) {
    // Do not replace symbol in f(x)=
    ExpressionModel::ReplaceSymbolWithUnknown(expressionToStore.childAtIndex(1), symbol);
  } else {
    // Fallback on normal parsing
    expressionToStore = ExpressionModel::buildExpressionFromText(c, symbol, context);
  }
  return expressionToStore;
}

void ContinuousFunction::Model::tidyDownstreamPoolFrom(char * treePoolCursor) const {
  if (treePoolCursor == nullptr || m_expressionDerivate.isDownstreamOf(treePoolCursor)) {
    m_numberOfSubCurves = 0;
    m_equationType = ComparisonNode::OperatorType::Equal;
    m_plotType = PlotType::Unknown;
    m_expressionDerivate = Expression();
  }
  ExpressionModel::tidyDownstreamPoolFrom(treePoolCursor);
}

void ContinuousFunction::Model::updatePlotType(const Ion::Storage::Record * record, const Expression equation, Context * context) const {
  // Named functions : PlotType has been updated when parsing the equation
  PlotType modelPlotType = plotType();

  assert(!equation.isUninitialized());
  if (equation.type() == ExpressionNode::Type::Undefined) {
    // Equation is undefined, preserve symbol.
    switch (modelPlotType) {
    case PlotType::Parametric:
      m_plotType = PlotType::UndefinedParametric;
      return;
    case PlotType::Polar:
      m_plotType = PlotType::UndefinedPolar;
      return;
    default:
      m_plotType = PlotType::Undefined;
      return;
    }
  }
  // Compute equation's degree regarding y and x.
  int yDeg = equation.polynomialDegree(context, k_ordinateName);
  int xDeg = equation.polynomialDegree(context, k_unknownName);

  // Inequations : equation symbol has been updated when parsing the equation
  ComparisonNode::OperatorType modelEquationType = equationType();

  if (modelPlotType == PlotType::Parametric || modelPlotType == PlotType::Polar || modelPlotType == PlotType::Cartesian) {
    // There should be no y symbol. Inequations are handled on cartesians only
    if (yDeg > 0 || (modelEquationType != ComparisonNode::OperatorType::Equal && modelPlotType != PlotType::Cartesian)) {
      // We distinguish the Unhandled type so that x/θ/t symbol is preserved.
      switch (modelPlotType) {
      case PlotType::Parametric:
        m_plotType = PlotType::UnhandledParametric;
        return;
      case PlotType::Polar:
        m_plotType = PlotType::UnhandledPolar;
        return;
      default:
        m_plotType = PlotType::Unhandled;
        return;
      }
    }
    if (ExamModeConfiguration::inequalityGraphingIsForbidden() && modelEquationType != ComparisonNode::OperatorType::Equal) {
      m_plotType = PlotType::Disabled;
      return;
    }
    if (modelPlotType == PlotType::Parametric) {
      const Expression matrixEquation = (equation.type()
                                         == ExpressionNode::Type::Dependency)
                                            ? equation.childAtIndex(0)
                                            : equation;
      if (matrixEquation.type() != ExpressionNode::Type::Matrix
          || static_cast<const Matrix &>(matrixEquation).numberOfRows() != 2
          || static_cast<const Matrix &>(matrixEquation).numberOfColumns()
                 != 1) {
        // Invalid parametric format
        m_plotType = PlotType::UnhandledParametric;
        return;
      }
    }
    // TODO : f(x)=1+x could be labelled as line.
    m_plotType = modelPlotType;
    return;
  }

  bool willBeAlongX = (yDeg == 1) || (yDeg == 2);
  bool willBeAlongY = !willBeAlongX && ((xDeg == 1) || (xDeg == 2));
  if (!willBeAlongX && !willBeAlongY) {
    // Any equation with such a y and x degree won't be handled anyway.
    m_plotType = PlotType::Unhandled;
    return;
  }

  const char * symbolName = willBeAlongX ? k_ordinateName : k_unknownName;
  TrinaryBoolean highestCoefficientIsPositive = TrinaryBoolean::Unknown;
  if (!HasNonNullCoefficients(equation, symbolName, context, &highestCoefficientIsPositive)
      || equation.hasComplexI(context)) {
    // The equation must have at least one nonNull coefficient.
    // TODO : Accept equations such as y=re(i)
    m_plotType = PlotType::Unhandled;
    return;
  }

  if (modelEquationType != ComparisonNode::OperatorType::Equal) {
    if (highestCoefficientIsPositive == TrinaryBoolean::Unknown || (yDeg == 2 && xDeg == -1)) {
      /* Are unhandled equation with :
       * - An unknown highest coefficient sign: sign must be strict and constant
       * - A non polynomial x coefficient in a quadratic equation on y. */
      m_plotType = PlotType::Unhandled;
      return;
    }
    if (ExamModeConfiguration::inequalityGraphingIsForbidden()) {
      m_plotType = PlotType::Disabled;
      return;
    }
    if (highestCoefficientIsPositive == TrinaryBoolean::False) {
      // Oppose the comparison operator
      m_equationType = ComparisonNode:: SwitchInferiorSuperior(modelEquationType);
    }
  }

  /* We can now rely on x and y degree to identify plot type :
   * | y  | x  | Status
   * | 0  | 1  | Vertical Line
   * | 0  | 2  | Vertical Lines
   * | 1  | 0  | Horizontal Line
   * | 1  | 1  | Line
   * | 1  | *  | Cartesian
   * | 2  | 0  | Other (Two Horizontal Lines)
   * | 2  | 1  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | 2  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | *  | Other
   * | *  | 1  | CartesianAlongY
   *
   * Other cases should have been escaped above.
   */
  if (ExamModeConfiguration::implicitPlotsAreForbidden()) {
    // No need to replace any symbols in originalEquation().
    Expression inputEquation = originalEquation(record, UCodePointUnknown);
    CodePoint symbol = willBeAlongX ? k_ordinateCodePoint : UCodePointUnknown;
    if (!IsExplicitEquation(inputEquation, symbol)) {
      m_plotType = PlotType::Disabled;
      return;
    }
  }

  if (!willBeAlongX) {
    if (xDeg == 2) {
      m_plotType = PlotType::OtherAlongY;
    } else if (yDeg == 0) {
      m_plotType = PlotType::VerticalLine;
    } else {
      m_plotType = PlotType::CartesianAlongY;
    }
    return;
  }

  if (yDeg == 1 && xDeg == 0) {
    m_plotType = PlotType::HorizontalLine;
    return;
  }

  if (yDeg == 1 && xDeg == 1 && highestCoefficientIsPositive != TrinaryBoolean::Unknown) {
    // An Unknown y coefficient sign might mean it depends on x (y*x = ...)
    m_plotType = PlotType::Line;
    return;
  }

  if (yDeg >= 1 && xDeg >= 1 && xDeg <= 2 && !ExamModeConfiguration::implicitPlotsAreForbidden()) {
    /* If implicit plots are forbidden, ignore conics (such as y=x^2) to hide
     * details. Otherwise, try to identify a conic.
     * For instance, x*y=1 as an hyperbola. */
    Conic equationConic = Conic(equation, context, k_unknownName);
    Conic::Type ctype = equationConic.getConicType();
    if (ctype == Conic::Type::Hyperbola) {
      m_plotType = yDeg > 1 ? PlotType::Hyperbola : PlotType::CartesianHyperbola;
      return;
    } else if (ctype == Conic::Type::Parabola) {
      m_plotType = yDeg > 1 ? PlotType::Parabola : PlotType::CartesianParabola;
      return;
    } else if (ctype == Conic::Type::Ellipse) {
      m_plotType = PlotType::Ellipse;
      return;
    } else if (ctype == Conic::Type::Circle) {
      m_plotType = PlotType::Circle;
      return;
    }
    // A conic could not be identified.
  }

  if (yDeg == 1) {
    m_plotType = PlotType::Cartesian;
    return;
  }

  assert(yDeg == 2);
  // Unknown type that we are able to plot anyway.
  m_plotType = PlotType::Other;
  return;
}

bool ContinuousFunction::Model::HasNonNullCoefficients(const Expression equation, const char * symbolName, Context * context, TrinaryBoolean * highestDegreeCoefficientIsPositive) {
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Symbols will be replaced anyway to compute isNull
  int degree = equation.getPolynomialReducedCoefficients(
      symbolName, coefficients, context, ComplexFormat(), AngleUnit(),
      k_defaultUnitFormat,
      ExpressionNode::SymbolicComputation::
          ReplaceAllDefinedSymbolsWithDefinition);
  // Degree should be >= 0 but reduction failure may result in a -1 degree.
  assert(degree <= Expression::k_maxPolynomialDegree);
  if (highestDegreeCoefficientIsPositive != nullptr && degree >= 0) {
    TrinaryBoolean isPositive = coefficients[degree].isPositive(context);
    if (isPositive == TrinaryBoolean::Unknown) {
      // Approximate for a better estimation. Nan if coefficient depends on x/y.
      double approximation = coefficients[degree].approximateToScalar<double>(
          context, ComplexFormat(), AngleUnit());
      if (!std::isnan(approximation) && approximation != 0.0) {
        isPositive = BinaryToTrinaryBool(approximation > 0.0);
      }
    }
    *highestDegreeCoefficientIsPositive = isPositive;
  }
  // Look for a NonNull coefficient.
  for (int d = 0; d <= degree; d++) {
    TrinaryBoolean isNull = coefficients[d].isNull(context);
    if (isNull == TrinaryBoolean::False) {
      return true;
    }
    if (isNull == TrinaryBoolean::Unknown) {
      // Approximate for a better estimation. Nan if coefficient depends on x/y.
      double approximation = coefficients[d].approximateToScalar<double>(
          context, ComplexFormat(), AngleUnit());
      if (!std::isnan(approximation) && approximation != 0.0) {
        return true;
      }
    }
  }
  return false;
}

bool ContinuousFunction::Model::IsExplicitEquation(const Expression equation, CodePoint symbol) {
  /* An equation is explicit if it is a comparison between the given symbol and
   * something that does not depend on it. For example, using 'y' symbol:
   * y=1+x or y>x are explicit but y+1=x or y=x+2*y are implicit. */
  return equation.type() == ExpressionNode::Type::Comparison
         && equation.childAtIndex(0).isIdenticalTo(Symbol::Builder(symbol))
         && !equation.childAtIndex(1).hasExpression(
             [](const Expression e, const void * context) {
               const CodePoint * symbol = static_cast<const CodePoint *>(context);
               return e.isIdenticalTo(Symbol::Builder(*symbol));
             }, static_cast<const void *>(&symbol));
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
