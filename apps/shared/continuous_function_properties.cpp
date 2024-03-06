#include "continuous_function_properties.h"

#include <apps/shared/expression_display_permissions.h>
#include <poincare/addition.h>
#include <poincare/constant.h>
#include <poincare/division.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/trigonometry.h>

#include "continuous_function.h"

using namespace Poincare;

namespace Shared {

bool ContinuousFunctionProperties::parameterAtIndexIsEditable(int index) const {
  assert(isEnabled());
  assert(index < numberOfCurveParameters());
  CurveParameterType curveParameterType = getCurveParameterType();
  switch (curveParameterType) {
    case CurveParameterType::CartesianFunction:
    case CurveParameterType::Line:
      return true;
    case CurveParameterType::VerticalLine:
      return index == 1;
    case CurveParameterType::HorizontalLine:
    case CurveParameterType::Parametric:
    case CurveParameterType::Polar:
    case CurveParameterType::InversePolar:
      return index == 0;
    default:
      return false;
  }
}

bool ContinuousFunctionProperties::parameterAtIndexIsPreimage(int index) const {
  assert(isEnabled());
  assert(index < numberOfCurveParameters());
  CurveParameterType curveParameterType = getCurveParameterType();
  return (curveParameterType == CurveParameterType::CartesianFunction ||
          curveParameterType == CurveParameterType::Line) &&
         index == 1;
}

ContinuousFunctionProperties::AreaType ContinuousFunctionProperties::areaType()
    const {
  assert(isInitialized());
  if (!isEnabled() || equationType() == ComparisonNode::OperatorType::Equal) {
    return AreaType::None;
  }
  // To draw y^2>a, the area plotted should be Outside and not Above.
  if (equationType() == ComparisonNode::OperatorType::Inferior ||
      equationType() == ComparisonNode::OperatorType::InferiorEqual) {
    return isOfDegreeTwo() ? AreaType::Inside : AreaType::Below;
  }
  assert(equationType() == ComparisonNode::OperatorType::Superior ||
         equationType() == ComparisonNode::OperatorType::SuperiorEqual);
  return isOfDegreeTwo() ? AreaType::Outside : AreaType::Above;
}

CodePoint ContinuousFunctionProperties::symbol() const {
  switch (symbolType()) {
    case SymbolType::T:
      return k_parametricSymbol;
    case SymbolType::Theta:
      return k_polarSymbol;
    case SymbolType::Radius:
      return k_radiusSymbol;
    case SymbolType::NoSymbol:
      return k_cartesianSymbol;
    default:
      assert(symbolType() == SymbolType::X);
      return k_cartesianSymbol;
  }
}

I18n::Message ContinuousFunctionProperties::MessageForSymbolType(
    SymbolType symbolType) {
  switch (symbolType) {
    case SymbolType::T:
      return I18n::Message::T;
    case SymbolType::Theta:
      return I18n::Message::Theta;
    case SymbolType::Radius:
      return I18n::Message::R;
    case SymbolType::NoSymbol:
      return I18n::Message::Default;
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

void ContinuousFunctionProperties::setErrorStatusAndUpdateCaption(
    Status status) {
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

void ContinuousFunctionProperties::update(
    const Poincare::Expression reducedEquation,
    const Poincare::Expression inputEquation, Context* context,
    Preferences::ComplexFormat complexFormat,
    ComparisonNode::OperatorType precomputedOperatorType,
    SymbolType precomputedFunctionSymbol, bool isCartesianEquation) {
  reset();
  m_isInitialized = true;

  setSymbolType(precomputedFunctionSymbol);
  setEquationType(precomputedOperatorType);

  if (Preferences::SharedPreferences()->examMode().forbidInequalityGraphing() &&
      precomputedOperatorType != ComparisonNode::OperatorType::Equal) {
    setErrorStatusAndUpdateCaption(Status::Banned);
    return;
  }

  /* We do not care about reduced expression since it is never shown to the
   * user. We do not care (neither have) an approximate expression. Indeed we
   * only check display permissions for input expression.*/
  bool genericCaptionOnly =
      Shared::ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
          inputEquation, Expression(), Expression(), context);

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

    // If there is still a dependency, it means that the reduction failed.
    if (analyzedExpression.type() == ExpressionNode::Type::Dependency) {
      setErrorStatusAndUpdateCaption(Status::Unhandled);
      return;
    }
  }

  // Compute equation's degree regarding y.
  int yDeg = analyzedExpression.polynomialDegree(context, k_ordinateName);
  if (!isCartesianEquation) {
    // There should be no y symbol. Inequations are handled on cartesians only
    if (yDeg > 0 ||
        (precomputedOperatorType != ComparisonNode::OperatorType::Equal &&
         precomputedFunctionSymbol != SymbolType::X)) {
      setErrorStatusAndUpdateCaption(Status::Unhandled);
      return;
    }

    /* Matrices, lists and points are not handled except:
     * - lists for SymbolType::NoSymbol
     * - points for SymbolType::NoSymbol and SymbolType::T */
    if (analyzedExpression.deepIsMatrix(context, true, false) ||
        (precomputedFunctionSymbol != SymbolType::NoSymbol &&
         analyzedExpression.deepIsList(context)) ||
        (precomputedFunctionSymbol != SymbolType::NoSymbol &&
         precomputedFunctionSymbol != SymbolType::T &&
         analyzedExpression.recursivelyMatches([](const Expression e) {
           return e.type() == ExpressionNode::Type::Point;
         }))) {
      setErrorStatusAndUpdateCaption(Status::Undefined);
      return;
    }

    if (precomputedFunctionSymbol == SymbolType::X) {
      setCartesianFunctionProperties(analyzedExpression, context);
      if (genericCaptionOnly) {
        setCaption(I18n::Message::Function);
      }
      return;
    }

    assert(precomputedOperatorType == ComparisonNode::OperatorType::Equal);

    if (precomputedFunctionSymbol == SymbolType::T) {
      if (analyzedExpression.type() != ExpressionNode::Type::Point) {
        // Invalid parametric format
        setErrorStatusAndUpdateCaption(Status::Unhandled);
        return;
      }
      setParametricFunctionProperties(analyzedExpression, context,
                                      complexFormat);
      if (genericCaptionOnly) {
        setCaption(I18n::Message::ParametricEquationType);
      }
      return;
    }

    if (precomputedFunctionSymbol == SymbolType::Radius) {
      // TODO: Inverse polar could also be analyzed
      setCaption(I18n::Message::PolarEquationType);
      setCurveParameterType(CurveParameterType::InversePolar);
      return;
    }

    if (precomputedFunctionSymbol == SymbolType::NoSymbol) {
      setCaption(analyzedExpression.type() == ExpressionNode::Type::Point
                     ? I18n::Message::PointType
                     : I18n::Message::ListOfPointsType);
      setCurveParameterType(CurveParameterType::ScatterPlot);
      return;
    }

    assert(precomputedFunctionSymbol == SymbolType::Theta);
    setPolarFunctionProperties(analyzedExpression, context, complexFormat);
    if (genericCaptionOnly) {
      setCaption(I18n::Message::PolarEquationType);
    }
    return;
  }

  // Compute equation's degree regarding x.
  int xDeg =
      analyzedExpression.polynomialDegree(context, Function::k_unknownName);

  bool willBeAlongX = (yDeg == 1) || (yDeg == 2);
  bool willBeAlongY = !willBeAlongX && ((xDeg == 1) || (xDeg == 2));
  if (!willBeAlongX && !willBeAlongY) {
    // Any equation with such a y and x degree won't be handled anyway.
    setErrorStatusAndUpdateCaption(Status::Unhandled);
    return;
  }

  const char* symbolName =
      willBeAlongX ? k_ordinateName : Function::k_unknownName;
  TrinaryBoolean highestCoefficientIsPositive = TrinaryBoolean::Unknown;
  if (!HasNonNullCoefficients(analyzedExpression, symbolName, context,
                              complexFormat, &highestCoefficientIsPositive)) {
    // The equation must have at least one nonNull coefficient.
    setErrorStatusAndUpdateCaption(Status::Unhandled);
    return;
  }

  if (precomputedOperatorType != ComparisonNode::OperatorType::Equal) {
    if (highestCoefficientIsPositive == TrinaryBoolean::Unknown ||
        (yDeg == 2 && xDeg == -1)) {
      /* Are unhandled equation with :
       * - An unknown highest coefficient sign: sign must be strict and constant
       * - A non polynomial x coefficient in a quadratic equation on y. */
      setErrorStatusAndUpdateCaption(Status::Unhandled);
      return;
    }
    if (highestCoefficientIsPositive == TrinaryBoolean::False) {
      // Oppose the comparison operator
      precomputedOperatorType =
          ComparisonNode::SwitchInferiorSuperior(precomputedOperatorType);
      setEquationType(precomputedOperatorType);
    }
  }

  if (Preferences::SharedPreferences()->examMode().forbidImplicitPlots()) {
    CodePoint symbol = willBeAlongX ? k_ordinateSymbol : UCodePointUnknown;
    if (!IsExplicitEquation(inputEquation, symbol)) {
      setErrorStatusAndUpdateCaption(Status::Banned);
      return;
    }
  }

  setCartesianEquationProperties(analyzedExpression, context, complexFormat,
                                 xDeg, yDeg, highestCoefficientIsPositive);
  if (genericCaptionOnly) {
    setCaption(I18n::Message::Equation);
  }
}

void ContinuousFunctionProperties::setCartesianFunctionProperties(
    const Expression& analyzedExpression, Context* context) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(isEnabled() && isCartesian());

  setCurveParameterType(CurveParameterType::CartesianFunction);

  // f(x) = piecewise(...)
  if (analyzedExpression.deepIsOfType({ExpressionNode::Type::PiecewiseOperator},
                                      context)) {
    setCaption(I18n::Message::PiecewiseType);
    return;
  }

  int xDeg =
      analyzedExpression.polynomialDegree(context, Function::k_unknownName);
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
          [](const Expression& e, Context* context, const char* symbol) {
            return e.type() == ExpressionNode::Type::Logarithm &&
                   e.childAtIndex(0).polynomialDegree(context, symbol) == 1;
          },
          Function::k_unknownName)) {
    setCaption(I18n::Message::LogarithmicType);
    return;
  }

  // f(x) = a*exp(b*x+c) + d
  if (analyzedExpression.isLinearCombinationOfFunction(
          context,
          [](const Expression& e, Context* context, const char* symbol) {
            if (e.type() != ExpressionNode::Type::Power) {
              return false;
            }
            Expression base = e.childAtIndex(0);
#if 0  // TODO_PCJ
            return base.type() == ExpressionNode::Type::ConstantMaths &&
                   static_cast<Constant&>(base).isExponentialE() &&
                   e.childAtIndex(1).polynomialDegree(context, symbol) == 1;
#else
            assert(false);
            return false;
#endif
          },
          Function::k_unknownName)) {
    setCaption(I18n::Message::ExponentialType);
    return;
  }

  // f(x) = polynomial/polynomial
  if (analyzedExpression.isLinearCombinationOfFunction(
          context, &Expression::IsRationalFraction, Function::k_unknownName)) {
    setCaption(I18n::Message::RationalType);
    return;
  }

  // f(x) = a*cos(b*x+c) + d*sin(e*x+f) + g*tan(h*x+k) + z
  ReductionContext reductionContext =
      ReductionContext::DefaultReductionContextForAnalysis(context);
  // tan(x) is reduced to sin(x)/cos(x) unless the target is User
  reductionContext.setTarget(ReductionTarget::User);
  Expression userReducedExpression =
      analyzedExpression.cloneAndReduce(reductionContext);
  if (userReducedExpression.isLinearCombinationOfFunction(
          context,
          [](const Expression& e, Context* context, const char* symbol) {
            return Poincare::Trigonometry::IsDirectTrigonometryFunction(e) &&
                   e.childAtIndex(0).polynomialDegree(context, symbol) == 1;
          },
          Function::k_unknownName)) {
    setCaption(I18n::Message::TrigonometricType);
    return;
  }

  // Others
  setCaption(I18n::Message::Function);
}

void ContinuousFunctionProperties::setCartesianEquationProperties(
    const Poincare::Expression& analyzedExpression, Poincare::Context* context,
    Preferences::ComplexFormat complexFormat, int xDeg, int yDeg,
    TrinaryBoolean highestCoefficientIsPositive) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(isEnabled() && isCartesian());

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

  setCaption(I18n::Message::Equation);

  if (yDeg != 1 && yDeg != 2) {  // function is along Y
    setIsAlongY(true);
    setCaption(I18n::Message::Equation);
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

  if (yDeg == 1 && xDeg == 1 &&
      highestCoefficientIsPositive != TrinaryBoolean::Unknown) {
    // An Unknown y coefficient sign might mean it depends on x (y*x = ...)
    setCaption(I18n::Message::LineType);
    setCurveParameterType(CurveParameterType::Line);
    return;
  }

  setIsOfDegreeTwo(yDeg == 2);
  setCurveParameterType(yDeg == 2 ? CurveParameterType::Default
                                  : CurveParameterType::CartesianFunction);

  if (xDeg >= 1 && xDeg <= 2 &&
      !Preferences::SharedPreferences()->examMode().forbidImplicitPlots()) {
    /* If implicit plots are forbidden, ignore conics (such as y=x^2) to hide
     * details. Otherwise, try to identify a conic.
     * For instance, x*y=1 as an hyperbola. */
    CartesianConic equationConic = CartesianConic(
        analyzedExpression, context, complexFormat, Function::k_unknownName);
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

void ContinuousFunctionProperties::setPolarFunctionProperties(
    const Expression& analyzedExpression, Context* context,
    Preferences::ComplexFormat complexFormat) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(isEnabled() && isPolar());

  setCurveParameterType(CurveParameterType::Polar);

  /* Detect polar lines
   * 1/sinOrCos(theta + B) --> Line
   * 1/cos(theta) --> Vertical line
   * 1/cos(theta + pi/2) --> Horizontal line
   */
  ReductionContext reductionContext =
      ReductionContext::DefaultReductionContextForAnalysis(context);
  Expression denominator, numerator;
  if (analyzedExpression.type() == ExpressionNode::Type::Multiplication) {
#if 0  // TODO_PCJ
    static_cast<const Multiplication&>(analyzedExpression)
        .splitIntoNormalForm(numerator, denominator, reductionContext);
#else
    assert(false);
#endif
  } else if (analyzedExpression.type() == ExpressionNode::Type::Power &&
             analyzedExpression.childAtIndex(1).isMinusOne()) {
    denominator = analyzedExpression.childAtIndex(0);
  }

  double angle = 0.0;
  double coefficientBeforeTheta = 1.0;
  if ((numerator.isUninitialized() ||
       numerator.polynomialDegree(reductionContext.context(),
                                  Function::k_unknownName) == 0) &&
      !denominator.isUninitialized() &&
      Trigonometry::DetectLinearPatternOfCosOrSin(
          denominator, reductionContext, Function::k_unknownName, false,
          nullptr, &coefficientBeforeTheta, &angle) &&
      std::abs(coefficientBeforeTheta) == 1.0) {
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
  PolarConic conicProperties = PolarConic(
      analyzedExpression, context, complexFormat, Function::k_unknownName);
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

void ContinuousFunctionProperties::setParametricFunctionProperties(
    const Poincare::Expression& analyzedExpression, Poincare::Context* context,
    Preferences::ComplexFormat complexFormat) {
  assert(analyzedExpression.type() != ExpressionNode::Type::Dependency);
  assert(isEnabled() && isParametric());
  assert(analyzedExpression.type() == ExpressionNode::Type::Point);
  assert(analyzedExpression.numberOfChildren() == 2);
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
#if 0  // TODO_PCJ
  if (variableX.type() == ExpressionNode::Type::Addition) {
    static_cast<Addition&>(variableX).removeConstantTerms(
        context, Function::k_unknownName);
  }
#else
  assert(false);
#endif
  Expression variableY = yOfT.clone();
#if 0  // TODO_PCJ
  if (variableY.type() == ExpressionNode::Type::Addition) {
    static_cast<Addition&>(variableY).removeConstantTerms(
        context, Function::k_unknownName);
  }
#else
  assert(false);
#endif
  Expression quotient = Division::Builder(variableX, variableY);
  quotient = quotient.cloneAndReduce(
      ReductionContext::DefaultReductionContextForAnalysis(context));
  if (quotient.polynomialDegree(context, Function::k_unknownName) == 0) {
    setCaption(I18n::Message::ParametricLineType);
    return;
  }

  // Detect polar conics
  ParametricConic conicProperties = ParametricConic(
      analyzedExpression, context, complexFormat, Function::k_unknownName);
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

bool ContinuousFunctionProperties::IsExplicitEquation(const Expression equation,
                                                      CodePoint symbol) {
  /* An equation is explicit if it is a comparison between the given symbol and
   * something that does not depend on it. For example, using 'y' symbol:
   * y=1+x or y>x are explicit but y+1=x or y=x+2*y are implicit. */
#if 1  // TODO_PCJ
  assert(false);
  return false;
#else
  return equation.type() == ExpressionNode::Type::Comparison &&
         equation.childAtIndex(0).isIdenticalTo(Symbol::Builder(symbol)) &&
         !equation.childAtIndex(1).recursivelyMatches(
             [](const Expression e, Context* context, void* auxiliary) {
               const CodePoint* symbol =
                   static_cast<const CodePoint*>(auxiliary);
               return (!e.isUninitialized() &&
                       e.isIdenticalTo(Symbol::Builder(*symbol)))
                          ? TrinaryBoolean::True
                          : TrinaryBoolean::Unknown;
             },
             nullptr, SymbolicComputation::DoNotReplaceAnySymbol,
             static_cast<void*>(&symbol));
#endif
}

bool ContinuousFunctionProperties::HasNonNullCoefficients(
    const Expression equation, const char* symbolName, Context* context,
    Preferences::ComplexFormat complexFormat,
    TrinaryBoolean* highestDegreeCoefficientIsPositive) {
  Preferences::AngleUnit angleUnit =
      Preferences::SharedPreferences()->angleUnit();
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Symbols will be replaced anyway to compute isNull
  int degree = equation.getPolynomialReducedCoefficients(
      symbolName, coefficients, context, complexFormat, angleUnit,
      k_defaultUnitFormat,
      SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  // Degree should be >= 0 but reduction failure may result in a -1 degree.
  assert(degree <= Expression::k_maxPolynomialDegree);
  ApproximationContext approximationContext(context, complexFormat, angleUnit);
  if (highestDegreeCoefficientIsPositive != nullptr && degree >= 0) {
    TrinaryBoolean isPositive = coefficients[degree].isPositive(context);
    if (isPositive == TrinaryBoolean::Unknown) {
      // Approximate for a better estimation. Nan if coefficient depends on x/y.
      double approximation = coefficients[degree].approximateToScalar<double>(
          approximationContext);
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
      double approximation =
          coefficients[d].approximateToScalar<double>(approximationContext);
      if (!std::isnan(approximation) && approximation != 0.0) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace Shared
