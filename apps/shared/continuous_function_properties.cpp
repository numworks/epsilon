#include "continuous_function_properties.h"
#include "continuous_function.h"
#include <poincare/constant.h>
#include <apps/exam_mode_configuration.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/trigonometry.h>
#include <apps/shared/expression_display_permissions.h>

using namespace Poincare;

namespace Shared {

ContinuousFunctionProperties::CurveParameter ContinuousFunctionProperties::getCurveParameter(int index) const {
  assert(canBeActive());
  using namespace I18n;
  switch(getCurveParameterType()) {
  case CurveParameterType::CartesianFunction:
    return {index == 0 ? Message::X : Message::Default, true, .isPreimage = index == 1};
  case CurveParameterType::Line:
    return {index == 0 ? Message::X : Message::Y, true, .isPreimage = index == 1};
  case CurveParameterType::HorizontalLine:
    return {index == 0 ? Message::X : Message::Y, index == 0};
  case CurveParameterType::VerticalLine:
    return {index == 0 ? Message::X : Message::Y, index == 1};
  case CurveParameterType::Parametric:
    return {index == 0 ? Message::T : index == 1 ? Message::XOfT : Message::YOfT, index == 0};
  case CurveParameterType::Polar:
    return {index == 0 ? Message::Theta : Message::R, index == 0};
  default:
    // Conics
    return {index == 0 ? Message::X : Message::Y, false};
  }
}

ContinuousFunctionProperties::AreaType ContinuousFunctionProperties::areaType() const {
  assert(isInitialized());
  if (!canBeActive() || equationType() == ComparisonNode::OperatorType::Equal) {
    return AreaType::None;
  }
  // To draw y^2>a, the area plotted should be Outside and not Above.
  if (equationType() == ComparisonNode::OperatorType::Inferior || equationType() == ComparisonNode::OperatorType::InferiorEqual) {
    return isOfDegreeTwo() ? AreaType::Inside : AreaType::Below;
  }
  assert(equationType() == ComparisonNode::OperatorType::Superior || equationType() == ComparisonNode::OperatorType::SuperiorEqual);
  return isOfDegreeTwo() ? AreaType::Outside : AreaType::Above;
}

CodePoint ContinuousFunctionProperties::symbol() const {
  switch (symbolType()) {
  case SymbolType::T:
    return k_parametricSymbol;
  case SymbolType::Theta:
    return k_polarSymbol;
  default:
    assert(symbolType() == SymbolType::X);
    return k_cartesianSymbol;
  }
}

I18n::Message ContinuousFunctionProperties::MessageForSymbolType(SymbolType symbolType) {
  switch (symbolType) {
  case SymbolType::T:
    return I18n::Message::T;
  case SymbolType::Theta:
    return I18n::Message::Theta;
  default:
    assert(symbolType == SymbolType::X);
    return I18n::Message::X;
  }
}

void ContinuousFunctionProperties::reset() {
  m_isInitialized = false;
  setCaption(k_defaultCaption);
  setStatus(k_defaultStatus);
  setEquationType(k_defaultEquationType);
  setSymbolType(k_defaultSymbolType);
  setCurveParameterType(k_defaultCurveParameterType);
  setConicShape(k_defaultConicShape);
  setIsOfDegreeTwo(k_defaultIsOfDegreeTwo);
  setIsAlongY(k_defaultIsAlongY);
}

void ContinuousFunctionProperties::setErrorStatusAndUpdateCaption(Status status) {
  assert(status != Status::Enabled);
  setStatus(status);
  switch (status) {
  case Status::Banned:
    setCaption(I18n::Message::Disabled);
    return;
  case Status::Undefined:
    setCaption(I18n::Message::UndefinedType);
    return;
  default:
    assert(status == Status::Unhandled);
    setCaption(I18n::Message::UnhandledType);
    return;
  }
}

void ContinuousFunctionProperties::update(const Poincare::Expression reducedEquation, const Poincare::Expression inputEquation, Context * context, ComparisonNode::OperatorType precomputedOperatorType, SymbolType precomputedFunctionSymbol, bool isCartesianEquation) {
  reset();
  m_isInitialized = true;

  setSymbolType(precomputedFunctionSymbol);
  setEquationType(precomputedOperatorType);

  if (ExamModeConfiguration::inequalityGraphingIsForbidden() && precomputedOperatorType != ComparisonNode::OperatorType::Equal) {
    setErrorStatusAndUpdateCaption(Status::Banned);
    return;
  }

  bool genericCaptionOnly = !inputEquation.isUninitialized() && Shared::ExpressionDisplayPermissions::ShouldNeverDisplayReduction(inputEquation, context);;
  setHideDetails(genericCaptionOnly);

  assert(!reducedEquation.isUninitialized());
  if (reducedEquation.type() == ExpressionNode::Type::Undefined) {
    setErrorStatusAndUpdateCaption(Status::Undefined);
    return;
  }

  Expression analyzedExpression = reducedEquation;
  if (reducedEquation.type() == ExpressionNode::Type::Dependency) {
    // Do not handle dependencies for now.
    analyzedExpression = reducedEquation.childAtIndex(0);
  }

  // Compute equation's degree regarding y.
  int yDeg = analyzedExpression.polynomialDegree(context, k_ordinateName);
  if (!isCartesianEquation) {
    // There should be no y symbol. Inequations are handled on cartesians only
    if (yDeg > 0 || (precomputedOperatorType != ComparisonNode::OperatorType::Equal && precomputedFunctionSymbol != SymbolType::X)) {
      setErrorStatusAndUpdateCaption(Status::Unhandled);
      return;
    }

    if (precomputedFunctionSymbol == SymbolType::X) {
      if (analyzedExpression.deepIsMatrix(context) || analyzedExpression.deepIsList(context)) {
        setErrorStatusAndUpdateCaption(Status::Undefined);
        return;
      }
      setCartesianFunctionProperties(analyzedExpression, context);
      if (genericCaptionOnly) {
        setCaption(I18n::Message::FunctionType);
      }
      return;
    }

    assert(precomputedOperatorType == ComparisonNode::OperatorType::Equal);

    if (precomputedFunctionSymbol == SymbolType::T) {
      if (analyzedExpression.type() != ExpressionNode::Type::Matrix
          || static_cast<const Matrix &>(analyzedExpression).numberOfRows() != 2
          || static_cast<const Matrix &>(analyzedExpression).numberOfColumns() != 1) {
        // Invalid parametric format
        setErrorStatusAndUpdateCaption(Status::Unhandled);
        return;
      }
      if (analyzedExpression.hasMatrixOrListChild(context, false)) {
        /* Reduction might have failed.
         * When reduction doesn't fail, a matrix containing
         * a matrix or a list is reduced to undef. */
        setErrorStatusAndUpdateCaption(Status::Undefined);
        return;
      }
      setParametricFunctionProperties(analyzedExpression, context);
      if (genericCaptionOnly) {
        setCaption(I18n::Message::ParametricEquationType);
      }
      return;
    }

    assert(precomputedFunctionSymbol == SymbolType::Theta);
    setPolarFunctionProperties(analyzedExpression, context);
    if (genericCaptionOnly) {
      setCaption(I18n::Message::PolarEquationType);
    }
    return;
  }

  // Compute equation's degree regarding x.
  int xDeg = analyzedExpression.polynomialDegree(context, Function::k_unknownName);

  bool willBeAlongX = (yDeg == 1) || (yDeg == 2);
  bool willBeAlongY = !willBeAlongX && ((xDeg == 1) || (xDeg == 2));
  if (!willBeAlongX && !willBeAlongY) {
    // Any equation with such a y and x degree won't be handled anyway.
    setErrorStatusAndUpdateCaption(Status::Unhandled);
    return;
  }

  const char * symbolName = willBeAlongX ? k_ordinateName : Function::k_unknownName;
  TrinaryBoolean highestCoefficientIsPositive = TrinaryBoolean::Unknown;
  if (!HasNonNullCoefficients(analyzedExpression, symbolName, context, &highestCoefficientIsPositive)) {
    // The equation must have at least one nonNull coefficient.
    setErrorStatusAndUpdateCaption(Status::Unhandled);
    return;
  }

  if (precomputedOperatorType != ComparisonNode::OperatorType::Equal) {
    if (highestCoefficientIsPositive == TrinaryBoolean::Unknown || (yDeg == 2 && xDeg == -1)) {
      /* Are unhandled equation with :
       * - An unknown highest coefficient sign: sign must be strict and constant
       * - A non polynomial x coefficient in a quadratic equation on y. */
      setErrorStatusAndUpdateCaption(Status::Unhandled);
      return;
    }
    if (highestCoefficientIsPositive == TrinaryBoolean::False) {
      // Oppose the comparison operator
      precomputedOperatorType = ComparisonNode::SwitchInferiorSuperior(precomputedOperatorType);
      setEquationType(precomputedOperatorType);
    }
  }

  if (ExamModeConfiguration::implicitPlotsAreForbidden()) {
    CodePoint symbol = willBeAlongX ? k_ordinateSymbol : UCodePointUnknown;
    if (!IsExplicitEquation(inputEquation, symbol)) {
      setErrorStatusAndUpdateCaption(Status::Banned);
      return;
    }
  }

  setCartesianEquationProperties(analyzedExpression, context, xDeg, yDeg, highestCoefficientIsPositive);
  if (genericCaptionOnly) {
    setCaption(I18n::Message::EquationType);
  }
}

void ContinuousFunctionProperties::setCartesianFunctionProperties(const Expression& analyzedExpression, Context * context) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(status() == Status::Enabled && isCartesian());

  setCurveParameterType(CurveParameterType::CartesianFunction);

  // f(x) = piecewise(...)
  if (analyzedExpression.recursivelyMatches(
    [](const Expression e, Context * context) {
      return e.type() == ExpressionNode::Type::PiecewiseOperator;
    },
    context))
  {
    setCaption(I18n::Message::PiecewiseType);
    return;
  }

  int xDeg = analyzedExpression.polynomialDegree(context, Function::k_unknownName);
  // f(x) = a
  if (xDeg == 0) {
    setCaption(I18n::Message::ConstantType);
    return;
  }

  // f(x) = a*x + b
  if (xDeg == 1) {
    if (analyzedExpression.type() == ExpressionNode::Type::Addition) {
      setCaption(I18n::Message::AffineType);
    } else {
      setCaption(I18n::Message::LinearType);
    }
    return;
  }

  // f(x) = a*x^n + b*x^ + ... + z
  if (xDeg > 1) {
    setCaption(I18n::Message::PolynomialType);
    return;
  }

  // f(x) = a*logk(b*x+c) + d*logM(e*x+f) + ... + z
  if (analyzedExpression.isLinearCombinationOfFunction(
    context,
    [](const Expression& e, Context * context, const char * symbol) {
      return e.type() == ExpressionNode::Type::Logarithm
            && e.childAtIndex(0).polynomialDegree(context, symbol) == 1;
    },
    Function::k_unknownName))
  {
    setCaption(I18n::Message::LogarithmicType);
    return;
  }

  // f(x) = a*exp(b*x+c) + d
  if (analyzedExpression.isLinearCombinationOfFunction(
    context,
    [](const Expression& e, Context * context, const char * symbol) {
      if (e.type() != ExpressionNode::Type::Power) {
        return false;
      }
      Expression base = e.childAtIndex(0);
      return base.type() == ExpressionNode::Type::ConstantMaths
            && static_cast<Constant&>(base).isExponentialE()
            && e.childAtIndex(1).polynomialDegree(context, symbol) == 1;
    },
    Function::k_unknownName))
  {
    setCaption(I18n::Message::ExponentialType);
    return;
  }

  // f(x) = polynomial/polynomial
  if (analyzedExpression.isLinearCombinationOfFunction(context, &Expression::IsRationalFraction, Function::k_unknownName)) {
    setCaption(I18n::Message::RationalType);
    return;
  }

  // f(x) = a*cos(b*x+c) + d*sin(e*x+f) + g*tan(h*x+k) + z
  ReductionContext reductionContext = ReductionContext::DefaultReductionContextForAnalysis(context);
  // tan(x) is reduced to sin(x)/cos(x) unless the target is User
  reductionContext.setTarget(ReductionTarget::User);
  Expression userReducedExpression = analyzedExpression.cloneAndReduce(reductionContext);
  if (userReducedExpression.isLinearCombinationOfFunction(
    context,
    [](const Expression& e, Context * context, const char * symbol) {
      return (e.type() == ExpressionNode::Type::Cosine || e.type() == ExpressionNode::Type::Sine || e.type() == ExpressionNode::Type::Tangent)
            && e.childAtIndex(0).polynomialDegree(context, symbol) == 1;
    },
    Function::k_unknownName))
  {
    setCaption(I18n::Message::TrigonometricType);
    return;
  }

  // Others
  setCaption(I18n::Message::FunctionType);
}

void ContinuousFunctionProperties::setCartesianEquationProperties(const Poincare::Expression& analyzedExpression, Poincare::Context * context, int xDeg, int yDeg, TrinaryBoolean highestCoefficientIsPositive) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(status() == Status::Enabled && isCartesian());

  /* We can rely on x and y degree to identify plot type :
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
   * Other cases should have been escaped before.
   */

  setCaption(I18n::Message::EquationType);

  if (yDeg != 1 && yDeg != 2) { // function is along Y
    setIsAlongY(true);
    setCaption(I18n::Message::EquationType);
    if (xDeg == 2) {
      setIsOfDegreeTwo(true);
    } else if (yDeg == 0) {
      setCaption(I18n::Message::VerticalLineType);
      setCurveParameterType(CurveParameterType::VerticalLine);
    } else {
      assert(xDeg == 1);
    }
    return;
  }

  assert(yDeg == 2 || yDeg == 1);

  if (yDeg == 1 && xDeg == 0) {
    setCaption(I18n::Message::HorizontalLineType);
    setCurveParameterType(CurveParameterType::HorizontalLine);
    return;
  }

  if (yDeg == 1 && xDeg == 1 && highestCoefficientIsPositive != TrinaryBoolean::Unknown) {
    // An Unknown y coefficient sign might mean it depends on x (y*x = ...)
    setCaption(I18n::Message::LineType);
    setCurveParameterType(CurveParameterType::Line);
    return;
  }

  setIsOfDegreeTwo(yDeg == 2);
  setCurveParameterType(yDeg == 2 ? CurveParameterType::Default : CurveParameterType::CartesianFunction);

  if (xDeg >= 1 && xDeg <= 2 && !ExamModeConfiguration::implicitPlotsAreForbidden()) {
    /* If implicit plots are forbidden, ignore conics (such as y=x^2) to hide
     * details. Otherwise, try to identify a conic.
     * For instance, x*y=1 as an hyperbola. */
    CartesianConic equationConic = CartesianConic(analyzedExpression, context, Function::k_unknownName);
    setConicShape(equationConic.conicType().shape);
    switch (conicShape()) {
    case Conic::Shape::Hyperbola:
      setCaption(I18n::Message::HyperbolaType);
      return;
    case Conic::Shape::Parabola:
      setCaption(I18n::Message::ParabolaType);
      return;
    case Conic::Shape::Ellipse:
      setCaption(I18n::Message::EllipseType);
      return;
    case Conic::Shape::Circle:
      setCaption(I18n::Message::CircleType);
      return;
    default:;
      // A conic could not be identified.
    }
  }
}

void ContinuousFunctionProperties::setPolarFunctionProperties(const Expression& analyzedExpression, Context * context) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(status() == Status::Enabled && isPolar());

  setCurveParameterType(CurveParameterType::Polar);

  /* Detect polar lines
   * 1/sinOrCos(theta + B) --> Line
   * 1/cos(theta) --> Vertical line
   * 1/cos(theta + pi/2) --> Horizontal line
   */
  ReductionContext reductionContext = ReductionContext::DefaultReductionContextForAnalysis(context);
  Expression denominator, numerator;
  if (analyzedExpression.type() == ExpressionNode::Type::Multiplication) {
    static_cast<const Multiplication&>(analyzedExpression).splitIntoNormalForm(numerator, denominator, reductionContext);
  } else if (analyzedExpression.type() == ExpressionNode::Type::Power && analyzedExpression.childAtIndex(1).isMinusOne()) {
    denominator = analyzedExpression.childAtIndex(0);
  }

  double angle = 0.0;
  double coefficientBeforeTheta = 1.0;
  if ((numerator.isUninitialized() || numerator.polynomialDegree(reductionContext.context(), Function::k_unknownName) == 0) && !denominator.isUninitialized() && Trigonometry::DetectLinearPatternOfCosOrSin(denominator, reductionContext, Function::k_unknownName, false, nullptr, &coefficientBeforeTheta, &angle) && abs(coefficientBeforeTheta) == 1.0) {
    double positiveAngle = std::fabs(angle);
    if (positiveAngle == 0.0 || positiveAngle == M_PI) {
      setCaption(I18n::Message::PolarVerticalLineType);
      return;
    }
    if (positiveAngle == M_PI_2 || positiveAngle == M_PI + M_PI_2) {
      setCaption(I18n::Message::PolarHorizontalLineType);
      return;
    }
    setCaption(I18n::Message::PolarLineType);
    return;
  }

  // Detect polar conics
  PolarConic conicProperties = PolarConic(analyzedExpression, context, Function::k_unknownName);
  setConicShape(conicProperties.conicType().shape);
  switch (conicShape()) {
  case Conic::Shape::Hyperbola:
    setCaption(I18n::Message::PolarHyperbolaType);
    return;
  case Conic::Shape::Parabola:
    setCaption(I18n::Message::PolarParabolaType);
    return;
  case Conic::Shape::Ellipse:
    setCaption(I18n::Message::PolarEllipseType);
    return;
  case Conic::Shape::Circle:
    setCaption(I18n::Message::PolarCircleType);
    return;
  default:
    // A conic could not be identified.
    setCaption(I18n::Message::PolarEquationType);
  }
}

void ContinuousFunctionProperties::setParametricFunctionProperties(const Poincare::Expression& analyzedExpression, Poincare::Context * context) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(status() == Status::Enabled && isParametric());
  assert(analyzedExpression.type() == ExpressionNode::Type::Matrix
        && static_cast<const Matrix&>(analyzedExpression).numberOfColumns() == 1
        && static_cast<const Matrix&>(analyzedExpression).numberOfRows() == 2);
  assert(!analyzedExpression.hasMatrixOrListChild(context, false));

  setCurveParameterType(CurveParameterType::Parametric);
  setCaption(I18n::Message::ParametricEquationType);

  // Detect lines
  const Expression xOfT = analyzedExpression.childAtIndex(0);
  const Expression yOfT = analyzedExpression.childAtIndex(1);
  int degOfTinX = xOfT.polynomialDegree(context, Function::k_unknownName);
  int degOfTinY = yOfT.polynomialDegree(context, Function::k_unknownName);
  if (degOfTinX == 0) {
    if (degOfTinY == 0) {
      // The curve is a dot
      return;
    }
    /* The same text as cartesian equation is used because the caption
     * "Parametric equation of a vertical line" is too long to fit
     * the 37 max chars limit in every language.
     * This can be changed later if more chars are available. */
    setCaption(I18n::Message::VerticalLineType);
    return;
  }
  if (degOfTinY == 0) {
    assert(degOfTinX != 0);
    /* Same comment as above. */
    setCaption(I18n::Message::HorizontalLineType);
    return;
  }
  if (degOfTinX == 1 && degOfTinY == 1) {
    setCaption(I18n::Message::ParametricLineType);
    return;
  }
  assert(degOfTinX != 0 && degOfTinY != 0);
  Expression variableX = xOfT.clone();
  if (variableX.type() == ExpressionNode::Type::Addition) {
    static_cast<Addition&>(variableX).removeConstantTerms(context, Function::k_unknownName);
  }
  Expression variableY = yOfT.clone();
  if (variableY.type() == ExpressionNode::Type::Addition) {
    static_cast<Addition&>(variableY).removeConstantTerms(context, Function::k_unknownName);
  }
  Expression quotient = Division::Builder(variableX, variableY);
  quotient = quotient.cloneAndReduce(ReductionContext::DefaultReductionContextForAnalysis(context));
  if (quotient.polynomialDegree(context, Function::k_unknownName) == 0) {
    setCaption(I18n::Message::ParametricLineType);
    return;
  }

  // Detect polar conics
  ParametricConic conicProperties = ParametricConic(analyzedExpression, context, Function::k_unknownName);
  setConicShape(conicProperties.conicType().shape);
  switch (conicShape()) {
  case Conic::Shape::Hyperbola:
    // For now, these are not detected and there is no caption for it.
    assert(false);
    return;
  case Conic::Shape::Parabola:
    setCaption(I18n::Message::ParametricParabolaType);
    return;
  case Conic::Shape::Ellipse:
    setCaption(I18n::Message::ParametricEllipseType);
    return;
  case Conic::Shape::Circle:
    setCaption(I18n::Message::ParametricCircleType);
    return;
  default:;
    // A conic could not be identified.
  }
}

bool ContinuousFunctionProperties::IsExplicitEquation(const Expression equation, CodePoint symbol) {
  /* An equation is explicit if it is a comparison between the given symbol and
   * something that does not depend on it. For example, using 'y' symbol:
   * y=1+x or y>x are explicit but y+1=x or y=x+2*y are implicit. */
  return equation.type() == ExpressionNode::Type::Comparison
         && equation.childAtIndex(0).isIdenticalTo(Symbol::Builder(symbol))
         && !equation.childAtIndex(1).recursivelyMatches(
            [](const Expression e, Context * context, void * auxiliary) {
              const CodePoint * symbol = static_cast<const CodePoint *>(auxiliary);
              return (!e.isUninitialized() && e.isIdenticalTo(Symbol::Builder(*symbol))) ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
            },
            nullptr,
            SymbolicComputation::DoNotReplaceAnySymbol,
            static_cast<void *>(&symbol));
}

bool ContinuousFunctionProperties::HasNonNullCoefficients(const Expression equation, const char * symbolName, Context * context, TrinaryBoolean * highestDegreeCoefficientIsPositive) {
  Preferences::ComplexFormat complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(Preferences::sharedPreferences->complexFormat(), equation, context);
  Preferences::AngleUnit angleUnit = Preferences::sharedPreferences->angleUnit();
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Symbols will be replaced anyway to compute isNull
  int degree = equation.getPolynomialReducedCoefficients(
      symbolName, coefficients, context, complexFormat, angleUnit,
      k_defaultUnitFormat,
      SymbolicComputation::
          ReplaceAllDefinedSymbolsWithDefinition);
  // Degree should be >= 0 but reduction failure may result in a -1 degree.
  assert(degree <= Expression::k_maxPolynomialDegree);
  if (highestDegreeCoefficientIsPositive != nullptr && degree >= 0) {
    TrinaryBoolean isPositive = coefficients[degree].isPositive(context);
    if (isPositive == TrinaryBoolean::Unknown) {
      // Approximate for a better estimation. Nan if coefficient depends on x/y.
      double approximation = coefficients[degree].approximateToScalar<double>(
          context, complexFormat, angleUnit);
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
          context, complexFormat, angleUnit);
      if (!std::isnan(approximation) && approximation != 0.0) {
        return true;
      }
    }
  }
  return false;
}

}
