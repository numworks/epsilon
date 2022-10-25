#include "continuous_function_properties.h"
#include "continuous_function.h"
#include <poincare/constant.h>
#include <poincare/multiplication.h>

using namespace Poincare;

namespace Shared {

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
    return &k_piecewiseFunctionType;
  }

  int xDeg = analysedEquation.polynomialDegree(context, Function::k_unknownName);
  // f(x) = a
  if (xDeg == 0) {
    return &k_constantFunctionType;
  }

  // f(x) = a*x + b
  if (xDeg == 1) {
    // analysedEquation is already reduced
    return analysedEquation.type() == ExpressionNode::Type::Addition ? &k_affineFunctionType : &k_linearFunctionType;
  }

  // f(x) = a*x^n + b*x^ + ... + z
  if (xDeg > 1) {
    return &k_polynomialFunctionType;
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
    return &k_trigonometricFunctionType;
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
    return &k_logarithmicFunctionType;
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
    return &k_exponentialFunctionType;
  }

  // f(x) = polynomial/polynomial
  if (IsLinearCombinationOfPattern(analysedEquation, context, &IsRationalFunction)) {
    return &k_rationalFunctionType;
  }

  // Others
  return &k_cartesianFunctionType;
}

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

Conic::Type ContinuousFunctionProperties::conicType() const {
  if (!isConic()) {
    return Conic::Type::Unknown;
  }
  if (m_plotType == &k_circleEquation) {
    return Conic::Type::Circle;
  }
  if (m_plotType == &k_ellipseEquation) {
    return Conic::Type::Ellipse;
  }
  if (m_plotType == &k_hyperbolaEquationWithOneSubCurve || m_plotType == &k_hyperbolaEquationWithTwoSubCurves) {
    return Conic::Type::Hyperbola;
  }
  assert(m_plotType == &k_parabolaEquationWithOneSubCurve || m_plotType == &k_parabolaEquationWithTwoSubCurves);
  return Conic::Type::Parabola;
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

}
