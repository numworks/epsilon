#include "continuous_function_properties.h"
#include "continuous_function.h"
#include <poincare/constant.h>
#include <apps/exam_mode_configuration.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>

using namespace Poincare;

namespace Shared {

ContinuousFunctionProperties::CurveParameter ContinuousFunctionProperties::getCurveParameter(int index) const {
  assert(canBeActive());
  using namespace I18n;
  switch(getCurveParameterType()) {
  case FunctionType::CurveParameterType::CartesianFunction:
    return {index == 0 ? Message::X : Message::Default, true, .isPreimage = index == 1};
  case FunctionType::CurveParameterType::Line:
    return {index == 0 ? Message::X : Message::Y, true, .isPreimage = index == 1};
  case FunctionType::CurveParameterType::HorizontalLine:
    return {index == 0 ? Message::X : Message::Y, index == 0};
  case FunctionType::CurveParameterType::VerticalLine:
    return {index == 0 ? Message::X : Message::Y, index == 1};
  case FunctionType::CurveParameterType::Parametric:
    return {index == 0 ? Message::T : index == 1 ? Message::XOfT : Message::YOfT, index == 0};
  case FunctionType::CurveParameterType::Polar:
    return {index == 0 ? Message::Theta : Message::R, index == 0};
  default:
    // Conics
    return {index == 0 ? Message::X : Message::Y, false};
  }
}

ContinuousFunctionProperties::AreaType ContinuousFunctionProperties::areaType() const {
  assert(isInitialized());
  if (!canBeActive() || m_equationType == ComparisonNode::OperatorType::Equal) {
    return AreaType::None;
  }
  // To draw y^2>a, the area plotted should be Outside and not Above.
  if (m_equationType == ComparisonNode::OperatorType::Inferior || m_equationType == ComparisonNode::OperatorType::InferiorEqual) {
    return numberOfSubCurves() == 1 ? AreaType::Below : AreaType::Inside;
  }
  assert(m_equationType == ComparisonNode::OperatorType::Superior || m_equationType == ComparisonNode::OperatorType::SuperiorEqual);
  return numberOfSubCurves() == 1 ? AreaType::Above : AreaType::Outside;
}

CodePoint ContinuousFunctionProperties::symbol() const {
  switch (symbolType()) {
  case FunctionType::SymbolType::T:
    return k_parametricSymbol;
  case FunctionType::SymbolType::Theta:
    return k_polarSymbol;
  default:
    assert(symbolType() == FunctionType::SymbolType::X);
    return k_cartesianSymbol;
  }
}

I18n::Message ContinuousFunctionProperties::MessageForSymbolType(FunctionType::SymbolType symbolType) {
  switch (symbolType) {
  case FunctionType::SymbolType::T:
    return I18n::Message::T;
  case FunctionType::SymbolType::Theta:
    return I18n::Message::Theta;
  default:
    assert(symbolType == FunctionType::SymbolType::X);
    return I18n::Message::X;
  }
}

void ContinuousFunctionProperties::update(const Poincare::Expression reducedEquation, const Poincare::Expression inputEquation, Context * context, ComparisonNode::OperatorType precomputedOperatorType, FunctionType::SymbolType precomputedFunctionSymbol) {
 if (ExamModeConfiguration::inequalityGraphingIsForbidden() && precomputedOperatorType != ComparisonNode::OperatorType::Equal) {
    setFunctionType(&FunctionTypes::k_bannedFunctionType);
    return;
  }

  assert(!reducedEquation.isUninitialized());
  if (reducedEquation.type() == ExpressionNode::Type::Undefined) {
    // Equation is undefined, preserve symbol.
    switch (precomputedFunctionSymbol) {
    case FunctionType::SymbolType::T:
      setFunctionType(&FunctionTypes::k_undefinedParametricFunctionType);
      return;
    case FunctionType::SymbolType::Theta:
      setFunctionType(&FunctionTypes::k_undefinedPolarFunctionType);
      return;
    default:
      setFunctionType(&FunctionTypes::k_undefinedCartesianFunctionType);
      return;
    }
  }

  setEquationType(precomputedOperatorType);

  // Compute equation's degree regarding y.
  int yDeg = reducedEquation.polynomialDegree(context, k_ordinateName);

  /* Symbol was precomputed if the expression is a function of type "f(x)=",
   * "f(t)=" or "r=" (for polar functions). */
  if (precomputedFunctionSymbol != FunctionType::SymbolType::Unknown) {
    // There should be no y symbol. Inequations are handled on cartesians only
    if (yDeg > 0 || (precomputedOperatorType != ComparisonNode::OperatorType::Equal && precomputedFunctionSymbol != FunctionType::SymbolType::X)) {
      // We distinguish the Unhandled type so that x/θ/t symbol is preserved.
      switch (precomputedFunctionSymbol) {
      case FunctionType::SymbolType::T:
        setFunctionType(&FunctionTypes::k_unhandledParametricFunctionType);
        return;
      case FunctionType::SymbolType::Theta:
        setFunctionType(&FunctionTypes::k_unhandledPolarFunctionType);
        return;
      default:
        setFunctionType(&FunctionTypes::k_unhandledCartesianFunctionType);
        return;
      }
    }

    if (precomputedFunctionSymbol == FunctionType::SymbolType::X) {
      setFunctionType(ContinuousFunctionProperties::CartesianFunctionAnalysis(reducedEquation, context));
      return;
    }

    assert(precomputedOperatorType == ComparisonNode::OperatorType::Equal);

    if (precomputedFunctionSymbol == FunctionType::SymbolType::T) {
      const Expression matrixEquation = (reducedEquation.type()
                                         == ExpressionNode::Type::Dependency)
                                            ? reducedEquation.childAtIndex(0)
                                            : reducedEquation;
      if (matrixEquation.type() != ExpressionNode::Type::Matrix
          || static_cast<const Matrix &>(matrixEquation).numberOfRows() != 2
          || static_cast<const Matrix &>(matrixEquation).numberOfColumns()
                 != 1) {
        // Invalid parametric format
        setFunctionType(&FunctionTypes::k_unhandledParametricFunctionType);
        return;
      }
      setFunctionType(&FunctionTypes::k_parametricFunctionType);
      return;
    }

    assert(precomputedFunctionSymbol == FunctionType::SymbolType::Theta);
    setFunctionType(&FunctionTypes::k_polarFunctionType);
    return;
  }

  // Compute equation's degree regarding x.
  int xDeg = reducedEquation.polynomialDegree(context, Function::k_unknownName);

  bool willBeAlongX = (yDeg == 1) || (yDeg == 2);
  bool willBeAlongY = !willBeAlongX && ((xDeg == 1) || (xDeg == 2));
  if (!willBeAlongX && !willBeAlongY) {
    // Any equation with such a y and x degree won't be handled anyway.
    setFunctionType(&FunctionTypes::k_unhandledCartesianFunctionType);
    return;
  }

  const char * symbolName = willBeAlongX ? k_ordinateName : Function::k_unknownName;
  TrinaryBoolean highestCoefficientIsPositive = TrinaryBoolean::Unknown;
  if (!HasNonNullCoefficients(reducedEquation, symbolName, context, &highestCoefficientIsPositive)
      || reducedEquation.hasComplexI(context)) {
    // The equation must have at least one nonNull coefficient.
    // TODO : Accept equations such as y=re(i)
    setFunctionType(&FunctionTypes::k_unhandledCartesianFunctionType);
    return;
  }

  if (precomputedOperatorType != ComparisonNode::OperatorType::Equal) {
    if (highestCoefficientIsPositive == TrinaryBoolean::Unknown || (yDeg == 2 && xDeg == -1)) {
      /* Are unhandled equation with :
       * - An unknown highest coefficient sign: sign must be strict and constant
       * - A non polynomial x coefficient in a quadratic equation on y. */
      setFunctionType(&FunctionTypes::k_unhandledCartesianFunctionType);
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
      setFunctionType(&FunctionTypes::k_bannedFunctionType);
      return;
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


  if (!willBeAlongX) {
    if (xDeg == 2) {
      setFunctionType(&FunctionTypes::k_cartesianEquationAlongYWithTwoSubCurves);
      return;
    }
    if (yDeg == 0) {
      setFunctionType(&FunctionTypes::k_verticalLine);
      return;
    }
    assert(xDeg == 1);
    setFunctionType(&FunctionTypes::k_cartesianEquationAlongY);
    return;
  }

  if (yDeg == 1 && xDeg == 0) {
    setFunctionType(&FunctionTypes::k_horizontalLineEquation);
    return;
  }

  if (yDeg == 1 && xDeg == 1 && highestCoefficientIsPositive != TrinaryBoolean::Unknown) {
    // An Unknown y coefficient sign might mean it depends on x (y*x = ...)
    setFunctionType(&FunctionTypes::k_lineEquation);
    return;
  }

  if (yDeg >= 1 && xDeg >= 1 && xDeg <= 2 && !ExamModeConfiguration::implicitPlotsAreForbidden()) {
    /* If implicit plots are forbidden, ignore conics (such as y=x^2) to hide
     * details. Otherwise, try to identify a conic.
     * For instance, x*y=1 as an hyperbola. */
    Conic equationConic = Conic(reducedEquation, context, Function::k_unknownName);
    Conic::Type ctype = equationConic.getConicType();
    if (ctype == Conic::Type::Hyperbola) {
      setFunctionType(yDeg > 1 ? &FunctionTypes::k_hyperbolaEquationWithTwoSubCurves : &FunctionTypes::k_hyperbolaEquationWithOneSubCurve);
      return;
    } else if (ctype == Conic::Type::Parabola) {
      setFunctionType(yDeg > 1 ? &FunctionTypes::k_parabolaEquationWithTwoSubCurves : &FunctionTypes::k_parabolaEquationWithOneSubCurve);
      return;
    } else if (ctype == Conic::Type::Ellipse) {
      setFunctionType(&FunctionTypes::k_ellipseEquation);
      return;
    } else if (ctype == Conic::Type::Circle) {
      setFunctionType(&FunctionTypes::k_circleEquation);
      return;
    }
    // A conic could not be identified.
  }

  if (yDeg == 1) {
    setFunctionType(&FunctionTypes::k_simpleCartesianEquationType);
    return;
  }

  assert(yDeg == 2);
  // Unknown type that we are able to plot anyway.
  setFunctionType(&FunctionTypes::k_cartesianEquationWithTwoSubCurves);
  return;
}

typedef bool (*PatternTest)(const Expression& e, Context * context);
static bool IsLinearCombinationOfPattern(const Expression& e, Context * context, PatternTest testFunction) {
  if (testFunction(e, context) || e.polynomialDegree(context, Function::k_unknownName) == 0) {
    return true;
  }
  if (e.type() == ExpressionNode::Type::Addition) {
    int n = e.numberOfChildren();
    assert(n > 0);
    for (int i = 0; i < n; i++) {
      if (!IsLinearCombinationOfPattern(e.childAtIndex(i), context, testFunction)) {
        return false;
      }
    }
    return true;
  }
  if (e.type() == ExpressionNode::Type::Multiplication) {
    int n = e.numberOfChildren();
    assert(n > 0);
    bool patternHasBeenDetected = false;
    for (int i = 0; i < n; i++) {
      Expression currentChild = e.childAtIndex(i);
      bool childIsConstant = currentChild.polynomialDegree(context, Function::k_unknownName) == 0;
      bool isPattern = !childIsConstant && IsLinearCombinationOfPattern(currentChild, context, testFunction);
      if (patternHasBeenDetected && isPattern) {
        // There can't be a multiplication of the pattern with itslef
        return false;
      }
      if (!isPattern && !childIsConstant) {
        // The coefficients must have a degree 0
        return false;
      }
      patternHasBeenDetected = patternHasBeenDetected || isPattern;
    }
    return patternHasBeenDetected;
  }
  return false;
}

static bool IsRationalFunction(const Expression& e, Context * context) {
  if (e.type() != ExpressionNode::Type::Multiplication && e.type() != ExpressionNode::Type::Power) {
    return false;
  }
  Expression numerator, denominator;
  static_cast<const Multiplication&>(e).splitIntoNormalForm(numerator, denominator, ExpressionNode::ReductionContext::DefaultReductionContextForAnalysis(context));
  int denominatorDegree;
  assert(!numerator.isUninitialized());
  if (denominator.isUninitialized()) {
    denominatorDegree = 0;
  } else {
    denominatorDegree = denominator.polynomialDegree(context, Function::k_unknownName);
  }
  int numeratorDegree = numerator.polynomialDegree(context, Function::k_unknownName);
  return denominatorDegree >= 0 && numeratorDegree >= 0;
}

const FunctionType * ContinuousFunctionProperties::CartesianFunctionAnalysis(const Expression& equation, Context * context) {
  Expression analysedEquation = equation;
  if (equation.type() == ExpressionNode::Type::Dependency) {
    analysedEquation = equation.childAtIndex(0);
  }

  // f(x) = piecewise(...)
  if (analysedEquation.recursivelyMatches(
    [](const Expression e, Context * context) {
      return e.type() == ExpressionNode::Type::PiecewiseOperator;
    },
    context))
  {
    return &FunctionTypes::k_piecewiseFunctionType;
  }

  int xDeg = analysedEquation.polynomialDegree(context, Function::k_unknownName);
  // f(x) = a
  if (xDeg == 0) {
    return &FunctionTypes::k_constantFunctionType;
  }

  // f(x) = a*x + b
  if (xDeg == 1) {
    // analysedEquation is already reduced
    return analysedEquation.type() == ExpressionNode::Type::Addition ? &FunctionTypes::k_affineFunctionType : &FunctionTypes::k_linearFunctionType;
  }

  // f(x) = a*x^n + b*x^ + ... + z
  if (xDeg > 1) {
    return &FunctionTypes::k_polynomialFunctionType;
  }

  // f(x) = a*cos(b*x+c) + d*sin(e*x+f) + g*tan(h*x+k) + z
  if (IsLinearCombinationOfPattern(
    analysedEquation,
    context,
    [](const Expression& e, Context * context) {
      return (e.type() == ExpressionNode::Type::Cosine || e.type() == ExpressionNode::Type::Sine || e.type() == ExpressionNode::Type::Tangent)
            && e.childAtIndex(0).polynomialDegree(context, Function::k_unknownName) == 1;
    }))
  {
    return &FunctionTypes::k_trigonometricFunctionType;
  }

  // f(x) = a*logk(b*x+c) + d*logM(e*x+f) + ... + z
  if (IsLinearCombinationOfPattern(
    analysedEquation,
    context,
    [](const Expression& e, Context * context) {
      return e.type() == ExpressionNode::Type::Logarithm
            && e.childAtIndex(0).polynomialDegree(context, Function::k_unknownName) == 1;
    }))
  {
    return &FunctionTypes::k_logarithmicFunctionType;
  }

  // f(x) = a*exp(b*x+c) + d
  if (IsLinearCombinationOfPattern(
    analysedEquation,
    context,
    [](const Expression& e, Context * context) {
      if (e.type() != ExpressionNode::Type::Power) {
        return false;
      }
      Expression base = e.childAtIndex(0);
      return base.type() == ExpressionNode::Type::ConstantMaths
            && static_cast<Constant&>(base).isExponentialE()
            && e.childAtIndex(1).polynomialDegree(context, Function::k_unknownName) == 1;
    }))
  {
    return &FunctionTypes::k_exponentialFunctionType;
  }

  // f(x) = polynomial/polynomial
  if (IsLinearCombinationOfPattern(analysedEquation, context, &IsRationalFunction)) {
    return &FunctionTypes::k_rationalFunctionType;
  }

  // Others
  return &FunctionTypes::k_cartesianFunctionType;
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
