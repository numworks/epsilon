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

using namespace Poincare;

namespace Shared {

constexpr char ContinuousFunction::k_unknownName[2];
constexpr char ContinuousFunction::k_ordinateName[2];

ContinuousFunction ContinuousFunction::NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName) {
  static int s_colorIndex = 0;
  // Create the record
  RecordDataBuffer data(Escher::Palette::nextDataColor(&s_colorIndex));
  if (baseName == nullptr) {
    // TODO Hugo : Check this assert never happens
    assert(false);
    // Return if error
    return ContinuousFunction();
  }

  *error = Ion::Storage::sharedStorage()->createRecordWithExtension(baseName, Ion::Storage::funcExtension, &data, sizeof(data));
  if (*error != Ion::Storage::Record::ErrorStatus::None) {
    assert(false); // TODO Hugo : Check it never happens
    // Return if error
    return ContinuousFunction();
  }

  // Return the ContinuousFunction with the new record
  return ContinuousFunction(Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(baseName, Ion::Storage::funcExtension));
}

bool ContinuousFunction::isActive() const {
  return recordData()->isActive();
}

KDColor ContinuousFunction::color() const {
  return recordData()->color();
}

void ContinuousFunction::setActive(bool active) {
  recordData()->setActive(active);
  if (!active) {
    didBecomeInactive();
  }
}

bool ContinuousFunction::isNamed() const {
  return fullName() != nullptr && fullName()[0] != '?';
}

// TODO Hugo : Account for y^2 sign !
bool ContinuousFunction::drawSuperiorArea() const {
  ExpressionNode::Type eqSymbol = equationSymbol();
  return eqSymbol == ExpressionNode::Type::Superior || eqSymbol == ExpressionNode::Type::SuperiorEqual;
}

bool ContinuousFunction::drawInferiorArea() const {
  ExpressionNode::Type eqSymbol = equationSymbol();
  return eqSymbol == ExpressionNode::Type::Inferior || eqSymbol == ExpressionNode::Type::InferiorEqual;
}

bool ContinuousFunction::drawCurve() const {
  ExpressionNode::Type eqSymbol = equationSymbol();
  return eqSymbol == ExpressionNode::Type::SuperiorEqual || eqSymbol == ExpressionNode::Type::InferiorEqual || eqSymbol == ExpressionNode::Type::Equal;
}

int ContinuousFunction::yDegree(Context * context) const {
  return expressionEquation(context).polynomialDegree(context, k_ordinateName);
}

int ContinuousFunction::xDegree(Context * context) const {
  return expressionEquation(context).polynomialDegree(context, k_unknownName);
}

// Check if a coefficient of y is never null. Compute its sign.
bool ContinuousFunction::isYCoefficientNonNull(int yDeg, Poincare::Context * context, Poincare::ExpressionNode::Sign * coefficientSign) const {
  assert(yDeg >= 0);
  if (coefficientSign) {
    *coefficientSign = ExpressionNode::Sign::Unknown;
  }
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  Preferences::ComplexFormat complexFormat = Preferences::sharedPreferences()->complexFormat();
  Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  int dy = expressionEquation(context).getPolynomialReducedCoefficients(k_ordinateName, coefficients, context, complexFormat, angleUnit, Preferences::UnitFormat::Metric, ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol);
  assert(dy >= yDeg);
  ExpressionNode::NullStatus coefficientNullStatus = coefficients[yDeg].nullStatus(context);
  if (coefficientNullStatus == ExpressionNode::NullStatus::Null || coefficients[yDeg].polynomialDegree(context, k_unknownName) != 0) {
    // Coefficient may be null or depends on x (which may be null)
    // NOTE : We could handle cases where it depends on x but is never null
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

int ContinuousFunction::nameWithArgument(char * buffer, size_t bufferSize) {
  if (isNamed()) {
    int funcNameSize = SymbolAbstract::TruncateExtension(buffer, fullName(), bufferSize);
    assert(funcNameSize > 0);
    size_t result = funcNameSize;
    assert(result <= bufferSize);
    buffer[result++] = '(';
    assert(result <= bufferSize);
    assert(UTF8Decoder::CharSizeOfCodePoint(symbol()) <= 2);
    result += UTF8Decoder::CodePointToChars(symbol(), buffer+result, bufferSize-result);
    assert(result <= bufferSize);
    result += strlcpy(buffer+result, ")", bufferSize-result);
    return result;
  }
  return strlcpy(buffer, k_ordinateName, bufferSize);
}

I18n::Message ContinuousFunction::parameterMessageName() const {
  return ParameterMessageForSymbolType(symbolType());
}

I18n::Message ContinuousFunction::functionCategory() const {
  assert(static_cast<size_t>(plotType()) < k_numberOfPlotTypes);
  static const I18n::Message category[k_numberOfPlotTypes] = {
    I18n::Message::PolarType, I18n::Message::ParametricType,I18n::Message::CartesianType,  I18n::Message::LineType, I18n::Message::HorizontalLineType,
    I18n::Message::VerticalLineType, I18n::Message::InequationType, I18n::Message::ConicsType, I18n::Message::CircleType, I18n::Message::EllipseType,
    I18n::Message::ParabolaType, I18n::Message::HyperbolaType, I18n::Message::OtherType, I18n::Message::UndefinedType, I18n::Message::UnhandledType,
  };
  return category[static_cast<size_t>(plotType())];
}

int ContinuousFunction::detailsTotal() const {
  if (isNull()) {
    return 0;
  }
  assert(static_cast<size_t>(plotType()) < k_numberOfPlotTypes);
  static const int total[k_numberOfPlotTypes] = {
    0, 0, 0, 3, 0,
    0, 0, 0, 3, 6,
    3, 6, 0, 0, 0,
  };
  return total[static_cast<size_t>(plotType())];
}

I18n::Message ContinuousFunction::detailsTitle(int i) const {
  PlotType type = plotType();
  assert(static_cast<size_t>(type) < k_numberOfPlotTypes);
  assert(i < detailsTotal());
  if (type == PlotType::Line) {
    static const I18n::Message titles[3] = {
      I18n::Message::LineEquationTitle, I18n::Message::LineSlopeTitle, I18n::Message::LineYInterceptTitle,
    };
    return titles[i];
  }
  if (type == PlotType::Circle) {
    static const I18n::Message titles[3] = {
      I18n::Message::CircleRadiusTitle, I18n::Message::CenterAbscissaTitle, I18n::Message::CenterOrdinateTitle,
    };
    return titles[i];
  }
  if (type == PlotType::Ellipse) {
    static const I18n::Message titles[6] = {
      I18n::Message::EllipseSemiMajorAxisTitle, I18n::Message::EllipseSemiMinorAxisTitle, I18n::Message::LinearEccentricityTitle,
      I18n::Message::EccentricityTitle, I18n::Message::CenterAbscissaTitle, I18n::Message::CenterOrdinateTitle,
    };
    return titles[i];
  }
  if (type == PlotType::Parabola) {
    static const I18n::Message titles[3] = {
      I18n::Message::ParabolaParameterTitle, I18n::Message::ParabolaVertexAbscissaTitle, I18n::Message::ParabolaVertexOrdinateTitle,
    };
    return titles[i];
  }
  assert(type == PlotType::Hyperbola);
  static const I18n::Message titles[6] = {
    I18n::Message::HyperbolaSemiMajorAxisTitle, I18n::Message::HyperbolaSemiMinorAxisTitle, I18n::Message::LinearEccentricityTitle,
    I18n::Message::EccentricityTitle, I18n::Message::CenterAbscissaTitle, I18n::Message::CenterOrdinateTitle,
  };
  return titles[i];
}

I18n::Message ContinuousFunction::detailsDescription(int i) const {
  PlotType type = plotType();
  assert(static_cast<size_t>(type) < k_numberOfPlotTypes);
  assert(i < detailsTotal());
  if (type == PlotType::Line) {
    static const I18n::Message titles[3] = {
      I18n::Message::LineEquationDescription, I18n::Message::LineSlopeDescription, I18n::Message::LineYInterceptDescription,
    };
    return titles[i];
  }
  if (type == PlotType::Circle) {
    static const I18n::Message titles[3] = {
      I18n::Message::CircleRadiusDescription, I18n::Message::CenterAbscissaDescription, I18n::Message::CenterOrdinateDescription,
    };
    return titles[i];
  }
  if (type == PlotType::Ellipse) {
    static const I18n::Message titles[6] = {
      I18n::Message::EllipseSemiMajorAxisDescription, I18n::Message::EllipseSemiMinorAxisDescription, I18n::Message::LinearEccentricityDescription,
      I18n::Message::EccentricityDescription, I18n::Message::CenterAbscissaDescription, I18n::Message::CenterOrdinateDescription,
    };
    return titles[i];
  }
  if (type == PlotType::Parabola) {
    static const I18n::Message titles[3] = {
      I18n::Message::ParabolaParameterDescription, I18n::Message::ParabolaVertexAbscissaDescription, I18n::Message::ParabolaVertexOrdinateDescription,
    };
    return titles[i];
  }
  assert(type == PlotType::Hyperbola);
  static const I18n::Message titles[6] = {
    I18n::Message::HyperbolaSemiMajorAxisDescription, I18n::Message::HyperbolaSemiMinorAxisDescription, I18n::Message::LinearEccentricityDescription,
    I18n::Message::EccentricityDescription, I18n::Message::CenterAbscissaDescription, I18n::Message::CenterOrdinateDescription,
  };
  return titles[i];
}

double ContinuousFunction::detailsValue(int i) const {
  PlotType type = plotType();
  assert(static_cast<size_t>(type) < k_numberOfPlotTypes);
  assert(i < detailsTotal());
  if (type == PlotType::Line) {
    double titles[3] = {
      NAN, recordData()->getLine(0), recordData()->getLine(1),
    };
    return titles[i];
  }
  double cx, cy;
  const Conic conicRepresentation = recordData()->getConic();
  if (type == PlotType::Parabola) {
    conicRepresentation.getSummit(&cx, &cy);
  } else {
    conicRepresentation.getCenter(&cx, &cy);
  }
  if (type == PlotType::Circle) {
    double titles[3] = {
      conicRepresentation.getRadius(), cx, cy,
    };
    return titles[i];
  }
  if (type == PlotType::Ellipse) {
    double titles[6] = {
      conicRepresentation.getSemiMajorAxis(), conicRepresentation.getSemiMinorAxis(), conicRepresentation.getLinearEccentricity(),
      conicRepresentation.getEccentricity(), cx, cy,
    };
    return titles[i];
  }
  if (type == PlotType::Parabola) {
    double titles[3] = {
      conicRepresentation.getParameter(), cx, cy,
    };
    return titles[i];
  }
  assert(type == PlotType::Hyperbola);
  double titles[6] = {
    conicRepresentation.getSemiMajorAxis(), conicRepresentation.getSemiMinorAxis(), conicRepresentation.getLinearEccentricity(),
    conicRepresentation.getEccentricity(), cx, cy,
  };
  return titles[i];
}

CodePoint ContinuousFunction::symbol() const {
  switch (plotType()) {
  case PlotType::Parametric:
    return 't';
  case PlotType::Polar:
    return UCodePointGreekSmallLetterTheta;
  default:
    return 'x';
  }
}

Expression ContinuousFunction::Model::originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const {
  return Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record)).replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Symbol::Builder(symbol));
}

Expression ContinuousFunction::Model::expressionEquation(const Ion::Storage::Record * record, Context * context) const {
  // TODO Hugo : Add todo expressionReduced on circularity ?
  // TODO Hugo : Either memoize or limit calls to this method
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
  if (result.childAtIndex(0).type() == ExpressionNode::Type::Function && (
      result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder(UCodePointGreekSmallLetterTheta))
      || result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder('x'))
      || result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder('t'))
      )
    ) {
    // TODO Hugo : Improve that
    if (result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder(UCodePointGreekSmallLetterTheta))) {
      m_plotType = PlotType::Polar;
    } else if (result.childAtIndex(0).childAtIndex(0).isIdenticalTo(Symbol::Builder('x'))) {
      m_plotType = PlotType::Cartesian;
    } else {
      m_plotType = PlotType::Parametric;
    }
    // Named function
    result = result.childAtIndex(1);
  } else {
    result = Subtraction::Builder(result.childAtIndex(0), result.childAtIndex(1));
    // ExpressionNode::ReductionContext reductionContext = ExpressionNode::ReductionContext(
    //   context, Preferences::sharedPreferences()->complexFormat(),
    //   Preferences::sharedPreferences()->angleUnit(),
    //   GlobalPreferences::sharedGlobalPreferences()->unitFormat(),
    //   ExpressionNode::ReductionTarget::SystemForAnalysis,
    //   ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol);
    // result = result.reduce(reductionContext);
  }
  /* 'Simplify' routine might need to call expressionReduced on the very
    * same function. So we need to keep a valid result while executing
    * 'Simplify'. Thus, we use a temporary expression. */
  Expression tempExpression = result.clone();
  PoincareHelpers::Reduce(&tempExpression, context, ExpressionNode::ReductionTarget::SystemForAnalysis);
  // PoincareHelpers::Simplify(&tempExpression, context, ExpressionNode::ReductionTarget::SystemForApproximation);
  // simplify might return an uninitialized Expression if interrupted
  if (!tempExpression.isUninitialized()) {
    result = tempExpression;
  }
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
          int d = m_expression.getPolynomialReducedCoefficients(k_unknownName, coefficients, context, Preferences::ComplexFormat::Cartesian, Preferences::sharedPreferences()->angleUnit(), Preferences::UnitFormat::Metric, ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol);
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
        int d = m_expression.getPolynomialReducedCoefficients(k_ordinateName, coefficients, context, Preferences::ComplexFormat::Cartesian, Preferences::sharedPreferences()->angleUnit(), Preferences::UnitFormat::Metric, ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol);
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
            newExpr.addChildAtIndexInPlace(root1, 0, 0);
            newExpr.addChildAtIndexInPlace(root2, 1, 1);
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

void ContinuousFunction::updatePlotType(Preferences::AngleUnit angleUnit, Context * context) {
  // Compute plot type from expressions
  /* If of format f(...) = ... , handle this -> Cartesian, Polar, Parametric or Undefined
   * | y  | x  | Status
   * | 0  | 0  | Undefined
   * | 0  | 1  | Vertical Line
   * | 0  | 2  | Unhandled (Multiple vertical lines ? # TODO)
   * | 0  | +  | Unhandled (Multiple vertical lines ? # TODO)
   * | 1  | 0  | Horizontal Line
   * | 1  | 1  | Line
   * | 1  | 2  | Cartesian
   * | 1  | +  | Cartesian
   * | 2  | 0  | Unhandled (Multiple horizontal lines ? # TODO)
   * | 2  | 1  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | 2  | Circle, Ellipsis, Hyperbola, Parabola, Other
   * | 2  | +  | Other
   * | +  | 0  | Unhandled (Multiple horizontal lines ? # TODO)
   * | +  | 1  | Unhandled (Swap x and y ? # TODO)
   * | +  | 2  | Unhandled (Swap x and y ? # TODO)
   * | +  | +  | Unhandled
   */
  /* TODO Hugo : Here we need to compute expressionEquation to make sure we get the
   * equation symbol right. It could be improved. */
  // TODO Hugo : Improve how Cartesian, Polar and parametric curves are detected.
  Expression equation = expressionEquation(context);
  recordData()->setEquationSymbol(m_model.m_equationSymbol);
  if (m_model.m_plotType != PlotType::Undefined) {
    // Polar, parametric, or cartesian decided in expressionEquation.
    return recordData()->setPlotType(m_model.m_plotType);
  }

  int yDeg = yDegree(context);
  if (m_model.m_equationSymbol != ExpressionNode::Type::Equal) {
    ExpressionNode::Sign YSign;
    if ((yDeg == 1 || yDeg == 2) && isYCoefficientNonNull(yDeg, context, &YSign) && YSign != ExpressionNode::Sign::Unknown) {
      if (YSign == ExpressionNode::Sign::Negative) {
        // Oppose the comparison operator
        Poincare::ExpressionNode::Type newEquationSymbol = ComparisonOperator::Opposite(m_model.m_equationSymbol);
        m_model.m_equationSymbol = newEquationSymbol;
        recordData()->setEquationSymbol(newEquationSymbol);
      }
      return recordData()->setPlotType(PlotType::Inequation);
    } else {
      // TODO Hugo : Handle vertical lines inequations
      return recordData()->setPlotType(PlotType::Unhandled);
    }
  }

  int xDeg = xDegree(context);
  if (yDeg == 0 && xDeg == 0) {
    return recordData()->setPlotType(PlotType::Undefined);
  }
  if (yDeg == 0 && xDeg == 1) {
    return recordData()->setPlotType(PlotType::VerticalLine);
  }
  if (yDeg <= 0 || !isYCoefficientNonNull(yDeg, context)) {
    /* Any other case where yDeg is null isn't handled.
     * Same if y's highest degree term depends on x, or may be null. */
    return recordData()->setPlotType(PlotType::Unhandled);
  }
  if (yDeg == 1 && xDeg == 0) {
    return recordData()->setPlotType(PlotType::HorizontalLine);
  }
  if (yDeg == 1 && xDeg == 1) {
    Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
    Preferences::ComplexFormat complexFormat = Preferences::ComplexFormat::Cartesian;
    Poincare::Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
    Expression lineExpression = expressionReduced(context).clone();
    // Reduce to another target for coefficient analysis.
    PoincareHelpers::Reduce(&lineExpression, context, ExpressionNode::ReductionTarget::SystemForAnalysis);
    int d = lineExpression.getPolynomialReducedCoefficients(k_unknownName, coefficients, context, complexFormat, angleUnit, Preferences::UnitFormat::Metric, ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol);
    if (d == 1) {
      recordData()->setLine(coefficients[1].approximateToScalar<double>(
                                context, complexFormat, angleUnit),
                            coefficients[0].approximateToScalar<double>(
                                context, complexFormat, angleUnit));

      return recordData()->setPlotType(PlotType::Line);
    } // Otherwise, there probably was a x*y term in the equation.
  }
  if (yDeg == 1) {
    return recordData()->setPlotType(PlotType::Cartesian);
  }
  if (yDeg == 2 && (xDeg == 1 || xDeg == 2)) {
    Conic equationConic = Conic(equation, context, k_unknownName);
    Conic::Type ctype = equationConic.getConicType();
    recordData()->setConic(equationConic);
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
  if (yDeg == 2) {
    return recordData()->setPlotType(PlotType::Other);
  }
  return recordData()->setPlotType(PlotType::Unhandled);
}

I18n::Message ContinuousFunction::ParameterMessageForSymbolType(SymbolType symbolType) {
  switch (symbolType) {
  case SymbolType::T:
    return I18n::Message::T;
  case SymbolType::Theta:
    return I18n::Message::Theta;
  default:
    return I18n::Message::X;
  }
}

bool ContinuousFunction::displayDerivative() const {
  return recordData()->displayDerivative();
}

void ContinuousFunction::setDisplayDerivative(bool display) {
  return recordData()->setDisplayDerivative(display);
}

int ContinuousFunction::derivativeNameWithArgument(char * buffer, size_t bufferSize) {
  if (!isNamed()) {
    return strlcpy(buffer, "y'", bufferSize);
  }
  // Fill buffer with f(x). Keep size for derivative sign.
  int derivativeSize = UTF8Decoder::CharSizeOfCodePoint('\'');
  int numberOfChars = nameWithArgument(buffer, bufferSize - derivativeSize);
  assert(numberOfChars + derivativeSize < (int)bufferSize);
  char * firstParenthesis = const_cast<char *>(UTF8Helper::CodePointSearch(buffer, '('));
  if (!UTF8Helper::CodePointIs(firstParenthesis, '(')) {
    assert(false); // TODO Hugo : Check this assert never happens
    assert(firstParenthesis[0] == 0 && buffer - firstParenthesis > bufferSize);
    firstParenthesis[derivativeSize-1] = 0;
  } else {
    memmove(firstParenthesis + derivativeSize, firstParenthesis, numberOfChars - (firstParenthesis - buffer) + 1);
  }
  UTF8Decoder::CodePointToChars('\'', firstParenthesis, derivativeSize);
  return numberOfChars + derivativeSize;
}

double ContinuousFunction::approximateDerivative(double x, Context * context, int i) const {
  assert(isAlongX());
  PlotType type = plotType();
  if (x < tMin() || x > tMax() || type == PlotType::VerticalLine) {
    return NAN;
  }
  // Derivative is simplified once and for all
  Expression derivate = expressionDerivateReduced(context);
  if (hasTwoCurves()) {
      assert(derivate.numberOfChildren() > i);
      /* TODO Hugo : !!! Ensure the roots are already sorted. It should be
       * possible once handling the sign of the coefficient of y^2.
       * Currently, derivatives make no sense. */
      assert(derivate.type() == Poincare::ExpressionNode::Type::Dependency);
      derivate = derivate.childAtIndex(0);
      derivate = derivate.childAtIndex(i);
  } else {
    assert(i == 0);
  }
  return PoincareHelpers::ApproximateWithValueForSymbol(derivate, k_unknownName, x, context);
}

int ContinuousFunction::printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Context * context) {
  // TODO Hugo : Re-check
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

bool ContinuousFunction::shouldClipTRangeToXRange() const {
  return isAlongX();
}

void ContinuousFunction::protectedFullRangeForDisplay(float tMin, float tMax, float tStep, float * min, float * max, Context * context, bool xRange) const {
  assert(!hasTwoCurves());
  Zoom::ValueAtAbscissa evaluation;
  if (xRange) {
    evaluation = [](float x, Context * context, const void * auxiliary) {
      return static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context).x1();
    };
  } else {
    evaluation = [](float x, Context * context, const void * auxiliary) {
      return static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context).x2();
    };
  }

  Zoom::FullRange(evaluation, tMin, tMax, tStep, min, max, context, this);
}

float ContinuousFunction::tMin() const {
  return recordData()->tMin();
}

float ContinuousFunction::tMax() const {
  return recordData()->tMax();
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

float ContinuousFunction::rangeStep() const {
  return isAlongX() ? NAN : (tMax() - tMin())/k_polarParamRangeSearchNumberOfPoints;
}

void ContinuousFunction::xRangeForDisplay(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic, Context * context) const {
  if (!isAlongX()) {
    assert(std::isfinite(tMin()) && std::isfinite(tMax()) && std::isfinite(rangeStep()) && rangeStep() > 0);
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
    return precision * std::round(static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context).x2() / precision);
  };
  Zoom::InterestingRangesForDisplay(evaluation, xMin, xMax, yMinIntrinsic, yMaxIntrinsic, std::max(tMin(), xMinLimit), std::min(tMax(), xMaxLimit), context, this);

  if (hasTwoCurves()) {
    float xMinTemp = *xMin;
    float xMaxTemp = *xMax;
    float yMinTemp = *yMinIntrinsic;
    float yMaxTemp = *yMaxIntrinsic;

    *xMin = NAN;
    *xMax = NAN;
    *yMinIntrinsic = NAN;
    *yMaxIntrinsic = NAN;

    Zoom::ValueAtAbscissa evaluation2 = [](float x, Context * context, const void * auxiliary) {
      /* When evaluating sin(x)/x close to zero using the standard sine function,
      * one can detect small variations, while the cardinal sine is supposed to
      * be locally monotonous. To smooth out such variations, we round the
      * result of the evaluations. As we are not interested in precise results
      * but only in ordering, this approximation is sufficient. */
      constexpr float precision = 1e-5;

      return precision * std::round(static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context, 1).x2() / precision);
    };
    Zoom::InterestingRangesForDisplay(evaluation2, xMin, xMax, yMinIntrinsic, yMaxIntrinsic, std::max(tMin(), xMinLimit), std::min(tMax(), xMaxLimit), context, this);

    Zoom::CombineRanges(xMinTemp, xMaxTemp, *xMin, *xMax, xMin, xMax);
    Zoom::CombineRanges(yMinTemp, yMaxTemp, *yMinIntrinsic, *yMaxIntrinsic, yMinIntrinsic, yMaxIntrinsic);
  }
}

void ContinuousFunction::yRangeForDisplay(float xMin, float xMax, float yMinForced, float yMaxForced, float ratio, float * yMin, float * yMax, Context * context, bool optimizeRange) const {
  if (!isAlongX()) {
    assert(std::isfinite(tMin()) && std::isfinite(tMax()) && std::isfinite(rangeStep()) && rangeStep() > 0);
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
    protectedFullRangeForDisplay(xMin, xMax, (xMax - xMin) / (Ion::Display::Width / 4), yMin, yMax, context, false);
    return;
  }

  Zoom::ValueAtAbscissa evaluation = [](float x, Context * context, const void * auxiliary) {
    return static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context).x2();
  };

  /* TODO Hugo : To handle second curve here, we would need to update
   * RangeWithRatioForDisplay so that it handles two evaluations. Otherwise,
   * preserving the ratio with a fixed xMin xMax with two curves is hard. */
  if (yMaxForced - yMinForced <= ratio * (xMax - xMin) && !hasTwoCurves()) {
    Zoom::RangeWithRatioForDisplay(evaluation, ratio, xMin, xMax, yMinForced, yMaxForced, yMin, yMax, context, this);
    // if (hasTwoCurves()) {
    //   float yMinTemp = *yMin;
    //   float yMaxTemp = *yMax;
    //   *yMin = NAN;
    //   *yMax = NAN;
    //   Zoom::ValueAtAbscissa evaluation2 = [](float x, Context * context, const void * auxiliary) {
    //     return static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context, 1).x2();
    //   };
    //   Zoom::RangeWithRatioForDisplay(evaluation2, ratio, xMin, xMax, yMinForced, yMaxForced, yMin, yMax, context, this);
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

    Zoom::ValueAtAbscissa evaluation2 = [](float x, Context * context, const void * auxiliary) {
      return static_cast<const ContinuousFunction *>(auxiliary)->evaluateXYAtParameter(x, context, 1).x2();
    };
    Zoom::RefinedYRangeForDisplay(evaluation2, xMin, xMax, yMin, yMax, context, this);

    Zoom::CombineRanges(yMinTemp, yMaxTemp, *yMin, *yMax, yMin, yMax);
  }
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

Expression ContinuousFunction::Model::expressionDerivateReduced(const Ion::Storage::Record * record, Poincare::Context * context) const {
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

template<typename T>
Coordinate2D<T> ContinuousFunction::templatedApproximateAtParameter(T t, Context * context, int i) const {
  if (t < tMin() || t > tMax()) {
    return Coordinate2D<T>(isAlongX() ? t : NAN, NAN);
  }
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknown[bufferSize];
  SerializationHelper::CodePoint(unknown, bufferSize, UCodePointUnknown);
  PlotType type = plotType();
  Expression e = expressionReduced(context);
  if (type != PlotType::Parametric) {
    if (hasTwoCurves()) {
      assert(e.numberOfChildren() > i);
      /* TODO Hugo : Ensure the roots are already sorted. It should be possible
       * once handling the sign of the coefficient of y^2. */
      if (i == 0) {
        return Coordinate2D<T>(t, std::max(PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(0), unknown, t, context), PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(1), unknown, t, context)));
      } else {
        return Coordinate2D<T>(t, std::min(PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(0), unknown, t, context), PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(1), unknown, t, context)));
      }
    } else {
      assert(i == 0);
    }
    if (type == PlotType::VerticalLine) {
      // Invert x and y with vertical lines so it can be scrolled vertically
      return Coordinate2D<T>(PoincareHelpers::ApproximateWithValueForSymbol(e, unknown, t, context), t);
    }
    return Coordinate2D<T>(t, PoincareHelpers::ApproximateWithValueForSymbol(e, unknown, t, context));
  }
  if (e.type() == ExpressionNode::Type::Dependency) {
    e = e.childAtIndex(0);
  }
  assert(e.type() == ExpressionNode::Type::Matrix);
  assert(static_cast<Matrix&>(e).numberOfRows() == 2);
  assert(static_cast<Matrix&>(e).numberOfColumns() == 1);
  return Coordinate2D<T>(
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(0), unknown, t, context),
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(1), unknown, t, context));
}

Coordinate2D<double> ContinuousFunction::nextMinimumFrom(double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) const {
  return nextPointOfInterestFrom(start, max, context, [](Expression e, char * symbol, double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) { return PoincareHelpers::NextMinimum(e, symbol, start, max, context, relativePrecision, minimalStep, maximalStep); }, relativePrecision, minimalStep, maximalStep);
}

Coordinate2D<double> ContinuousFunction::nextMaximumFrom(double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) const {
  return nextPointOfInterestFrom(start, max, context, [](Expression e, char * symbol, double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) { return PoincareHelpers::NextMaximum(e, symbol, start, max, context, relativePrecision, minimalStep, maximalStep); }, relativePrecision, minimalStep, maximalStep);
}

Coordinate2D<double> ContinuousFunction::nextRootFrom(double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) const {
  return nextPointOfInterestFrom(start, max, context, [](Expression e, char * symbol, double start, double max, Context * context, double relativePrecision, double minimalStep, double maximalStep) { return Coordinate2D<double>(PoincareHelpers::NextRoot(e, symbol, start, max, context, relativePrecision, minimalStep, maximalStep), 0.0); }, relativePrecision, minimalStep, maximalStep);
}

Coordinate2D<double> ContinuousFunction::nextIntersectionFrom(double start, double max, Context * context, Expression e, double relativePrecision, double minimalStep, double maximalStep, double eDomainMin, double eDomainMax) const {
  assert(isAlongX());
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknown);
  double tmin = std::max<double>(tMin(), eDomainMin), tmax = std::min<double>(tMax(), eDomainMax);
  start = start < tmin ? tmin : start > tmax ? tmax : start;
  max = max < tmin ? tmin : max > tmax ? tmax : max;
  if (start == max) {
    return NAN;
  }
  return PoincareHelpers::NextIntersection(expressionReduced(context), unknownX, start, max, context, e, relativePrecision, minimalStep, maximalStep);
}

Coordinate2D<double> ContinuousFunction::nextPointOfInterestFrom(double start, double max, Context * context, ComputePointOfInterest compute, double relativePrecision, double minimalStep, double maximalStep) const {
  assert(isAlongX());
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknown);
  double tmin = tMin(), tmax = tMax();
  start = start < tmin ? tmin : start > tmax ? tmax : start;
  max = max < tmin ? tmin : max > tmax ? tmax : max;
  if (start == max) {
    return NAN;
  }
  return compute(expressionReduced(context), unknownX, start, max, context, relativePrecision, minimalStep, maximalStep);
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

Ion::Storage::Record::ErrorStatus ContinuousFunction::setContent(const char * c, Poincare::Context * context) {
  Ion::Storage::Record::ErrorStatus error = ExpressionModelHandle::setContent(c, context);
  if (error == Ion::Storage::Record::ErrorStatus::None && !isNull()) {
    bool previousAlongXStatus = isAlongX();
    updatePlotType(Preferences::AngleUnit::Radian, context);
    if (previousAlongXStatus != isAlongX()) {
      // Recompute the definition's domain
      setTMin(!isAlongX() ? 0.0 : -INFINITY);
      setTMax(!isAlongX() ? 2.0*Trigonometry::PiInAngleUnit(Preferences::sharedPreferences()->angleUnit()) : INFINITY);
    }
  }
  return error;
}

bool ContinuousFunction::basedOnCostlyAlgorithms(Context * context) const {
  return expressionReduced(context).hasExpression([](const Expression e, const void * context) {
      return e.type() == ExpressionNode::Type::Sequence
          || e.type() == ExpressionNode::Type::Integral
          || e.type() == ExpressionNode::Type::Derivative;
      }, nullptr);
}

template <typename T>
Poincare::Coordinate2D<T> ContinuousFunction::privateEvaluateXYAtParameter(T t, Poincare::Context * context, int i) const {
  Coordinate2D<T> x1x2 = templatedApproximateAtParameter(t, context, i);
  PlotType type = plotType();
  if (type != PlotType::Polar) {
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

template Coordinate2D<float> ContinuousFunction::templatedApproximateAtParameter<float>(float, Context *, int) const;
template Coordinate2D<double> ContinuousFunction::templatedApproximateAtParameter<double>(double, Context *, int) const;

template Coordinate2D<float> ContinuousFunction::privateEvaluateXYAtParameter<float>(float, Context *, int) const;
template Coordinate2D<double> ContinuousFunction::privateEvaluateXYAtParameter<double>(double, Context *, int) const;


} // namespace Graph
