#include "continuous_function_properties.h"
#include "continuous_function.h"
#include <poincare/constant.h>
#include <apps/exam_mode_configuration.h>
#include <poincare/division.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/trigonometry.h>

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
    return numberOfSubCurves() == 1 ? AreaType::Below : AreaType::Inside;
  }
  assert(equationType() == ComparisonNode::OperatorType::Superior || equationType() == ComparisonNode::OperatorType::SuperiorEqual);
  return numberOfSubCurves() == 1 ? AreaType::Above : AreaType::Outside;
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
  setHasTwoSubCurves(k_defaultHasTwoSubCurves);
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

  assert(!reducedEquation.isUninitialized());
  if (reducedEquation.type() == ExpressionNode::Type::Undefined) {
    setErrorStatusAndUpdateCaption(Status::Undefined);
    return;
  }

  Expression analysedEquation = reducedEquation;
  if (reducedEquation.type() == ExpressionNode::Type::Dependency) {
    // Do not handle dependencies for now.
    analysedEquation = reducedEquation.childAtIndex(0);
  }

  // Compute equation's degree regarding y.
  int yDeg = analysedEquation.polynomialDegree(context, k_ordinateName);
  if (!isCartesianEquation) {
    // There should be no y symbol. Inequations are handled on cartesians only
    if (yDeg > 0 || (precomputedOperatorType != ComparisonNode::OperatorType::Equal && precomputedFunctionSymbol != SymbolType::X)) {
      setErrorStatusAndUpdateCaption(Status::Unhandled);
      return;
    }

    if (precomputedFunctionSymbol == SymbolType::X) {
      setCartesianFunctionProperties(analysedEquation, context);
      return;
    }

    assert(precomputedOperatorType == ComparisonNode::OperatorType::Equal);

    if (precomputedFunctionSymbol == SymbolType::T) {
      if (analysedEquation.type() != ExpressionNode::Type::Matrix
          || static_cast<const Matrix &>(analysedEquation).numberOfRows() != 2
          || static_cast<const Matrix &>(analysedEquation).numberOfColumns() != 1) {
        // Invalid parametric format
        setStatus(Status::Unhandled);
        return;
      }
      setParametricFunctionProperties(analysedEquation, context);
      return;
    }

    assert(precomputedFunctionSymbol == SymbolType::Theta);
    setPolarFunctionProperties(analysedEquation, context);
    return;
  }

  // Compute equation's degree regarding x.
  int xDeg = analysedEquation.polynomialDegree(context, Function::k_unknownName);

  bool willBeAlongX = (yDeg == 1) || (yDeg == 2);
  bool willBeAlongY = !willBeAlongX && ((xDeg == 1) || (xDeg == 2));
  if (!willBeAlongX && !willBeAlongY) {
    // Any equation with such a y and x degree won't be handled anyway.
    setErrorStatusAndUpdateCaption(Status::Unhandled);
    return;
  }

  const char * symbolName = willBeAlongX ? k_ordinateName : Function::k_unknownName;
  TrinaryBoolean highestCoefficientIsPositive = TrinaryBoolean::Unknown;
  if (!HasNonNullCoefficients(analysedEquation, symbolName, context, &highestCoefficientIsPositive)
      || analysedEquation.hasComplexI(context)) {
    // The equation must have at least one nonNull coefficient.
    // TODO : Accept equations such as y=re(i)
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

  setCartesianEquationProperties(analysedEquation, context, xDeg, yDeg, highestCoefficientIsPositive);
}

void ContinuousFunctionProperties::setCartesianFunctionProperties(const Expression& reducedEquation, Context * context) {
  assert(reducedEquation.type() != ExpressionNode::Type::Dependency);
  assert(status() == Status::Enabled && isCartesian());

  setCurveParameterType(CurveParameterType::CartesianFunction);

  // f(x) = piecewise(...)
  if (reducedEquation.recursivelyMatches(
    [](const Expression e, Context * context) {
      return e.type() == ExpressionNode::Type::PiecewiseOperator;
    },
    context))
  {
    setCaption(I18n::Message::PiecewiseType);
    return;
  }

  int xDeg = reducedEquation.polynomialDegree(context, Function::k_unknownName);
  // f(x) = a
  if (xDeg == 0) {
    setCaption(I18n::Message::ConstantType);
    return;
  }

  // f(x) = a*x + b
  if (xDeg == 1) {
    if (reducedEquation.type() == ExpressionNode::Type::Addition) {
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

  // f(x) = a*cos(b*x+c) + d*sin(e*x+f) + g*tan(h*x+k) + z
  if (reducedEquation.isLinearCombinationOfFunction(
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

  // f(x) = a*logk(b*x+c) + d*logM(e*x+f) + ... + z
  if (reducedEquation.isLinearCombinationOfFunction(
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
  if (reducedEquation.isLinearCombinationOfFunction(
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
  if (reducedEquation.isLinearCombinationOfFunction(context, &Expression::IsRationalFunction, Function::k_unknownName)) {
    setCaption(I18n::Message::RationalType);
    return;
  }

  // Others
  setCaption(I18n::Message::FunctionType);
}

void ContinuousFunctionProperties::setCartesianEquationProperties(const Poincare::Expression& reducedEquation, Poincare::Context * context, int xDeg, int yDeg, TrinaryBoolean highestCoefficientIsPositive) {
  assert(reducedEquation.type() != ExpressionNode::Type::Dependency);
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
      setHasTwoSubCurves(true);
    } else if (yDeg == 0) {
      setCaption(I18n::Message::VerticalLineType);
      setCurveParameterType(CurveParameterType::VerticalLine);
    } else {
      assert(xDeg == 1);
    }
    return;
  }

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

  setHasTwoSubCurves(yDeg > 1);
  setCurveParameterType(yDeg > 1 ? CurveParameterType::Default : CurveParameterType::CartesianFunction);

  if (yDeg >= 1 && xDeg >= 1 && xDeg <= 2 && !ExamModeConfiguration::implicitPlotsAreForbidden()) {
    /* If implicit plots are forbidden, ignore conics (such as y=x^2) to hide
     * details. Otherwise, try to identify a conic.
     * For instance, x*y=1 as an hyperbola. */
    CartesianConic equationConic = CartesianConic(reducedEquation, context, Function::k_unknownName);
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

void ContinuousFunctionProperties::setPolarFunctionProperties(const Expression& reducedEquation, Context * context) {
  assert(reducedEquation.type() != ExpressionNode::Type::Dependency);
  assert(status() == Status::Enabled && isPolar());

  setCurveParameterType(CurveParameterType::Polar);

  /* Detect polar lines
   * 1/sinOrCos(theta + B) --> Line
   * 1/cos(theta) --> Vertical line
   * 1/cos(theta + pi/2) --> Horizontal line
   *
   * TODO: Polar lines are badly plotted by the Grapher app so this is not
   * very useful.
   */
  ExpressionNode::ReductionContext reductionContext = ExpressionNode::ReductionContext::DefaultReductionContextForAnalysis(context);
  Expression denominator, numerator;
  if (reducedEquation.type() == ExpressionNode::Type::Multiplication) {
    static_cast<const Multiplication&>(reducedEquation).splitIntoNormalForm(numerator, denominator, reductionContext);
  } else if (reducedEquation.type() == ExpressionNode::Type::Power && reducedEquation.childAtIndex(1).isMinusOne()) {
    denominator = reducedEquation.childAtIndex(0);
  }

  double angle = 0.0;
  double coefficientBeforeTheta = 1.0;
  if (!denominator.isUninitialized() && Trigonometry::IsCosOrSinOfSymbol(denominator, reductionContext, Function::k_unknownName, false, nullptr, &coefficientBeforeTheta, &angle) && coefficientBeforeTheta == 1.0) {
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
  PolarConic conicProperties = PolarConic(reducedEquation, context, Function::k_unknownName);
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

void ContinuousFunctionProperties::setParametricFunctionProperties(const Poincare::Expression& reducedEquation, Poincare::Context * context) {
  assert(reducedEquation.type() != ExpressionNode::Type::Dependency);
  assert(status() == Status::Enabled && isParametric());
  assert(reducedEquation.type() == ExpressionNode::Type::Matrix
        && static_cast<const Matrix&>(reducedEquation).numberOfColumns() == 1
        && static_cast<const Matrix&>(reducedEquation).numberOfRows() == 2);

  setCurveParameterType(CurveParameterType::Parametric);

  // Detect lines
  const Expression xOfT = reducedEquation.childAtIndex(0);
  const Expression yOfT = reducedEquation.childAtIndex(1);
  int degOfTinX = xOfT.polynomialDegree(context, Function::k_unknownName);
  int degOfTinY = yOfT.polynomialDegree(context, Function::k_unknownName);
  if (degOfTinX == 0 && degOfTinY != 0) {
    setCaption(I18n::Message::ParametricVerticalLineType);
    return;
  }
  if (degOfTinY == 0 && degOfTinX != 0) {
    setCaption(I18n::Message::ParametricHorizontalLineType);
    return;
  }
  Expression quotient = Division::Builder(xOfT.clone(), yOfT.clone());
  quotient = quotient.cloneAndReduce(ExpressionNode::ReductionContext::DefaultReductionContextForAnalysis(context));
  // TODO: Some lines are not detected like (x, y) = (3*ln(t), 4*ln(t)+1)
  if (quotient.polynomialDegree(context, Function::k_unknownName) == 0) {
    setCaption(I18n::Message::ParametricLineType);
    return;
  }

  // Detect polar conics
  ParametricConic conicProperties = ParametricConic(reducedEquation, context, Function::k_unknownName);
  setConicShape(conicProperties.conicType().shape);
  switch (conicShape()) {
  case Conic::Shape::Hyperbola:
    setCaption(I18n::Message::ParametricHyperbolaType);
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
  default:
    // A conic could not be identified.
    setCaption(I18n::Message::ParametricEquationType);
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
            ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol,
            static_cast<void *>(&symbol));
}

bool ContinuousFunctionProperties::HasNonNullCoefficients(const Expression equation, const char * symbolName, Context * context, TrinaryBoolean * highestDegreeCoefficientIsPositive) {
  Preferences::ComplexFormat complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(Preferences::sharedPreferences()->complexFormat(), equation, context);
  Preferences::AngleUnit angleUnit = Preferences::UpdatedAngleUnitWithExpressionInput(Preferences::sharedPreferences()->angleUnit(), equation, context);
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Symbols will be replaced anyway to compute isNull
  int degree = equation.getPolynomialReducedCoefficients(
      symbolName, coefficients, context, complexFormat, angleUnit,
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
