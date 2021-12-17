#include "../app.h"
#include <apps/global_preferences.h>
#include "../../shared/poincare_helpers.h"
#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/rational.h>
#include <poincare/opposite.h>
#include <poincare/addition.h>
#include <poincare/parenthesis.h>
#include <poincare/equal.h>
#include <poincare/subtraction.h>
#include <poincare/multiplication.h>
#include <poincare/division.h>
#include <poincare/square_root.h>
#include <poincare/symbol.h>
#include <poincare/power.h>
#include "second_degree_list_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {
    
void SecondDegreeListController::setExpression(Poincare::Expression e) {
  ExpressionsListController::setExpression(e);
  assert(!m_expression.isUninitialized());

  Expression polynomialCoefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  
  Context * context = App::app()->localContext();
  Preferences * preferences = Preferences::sharedPreferences();
  Poincare::ExpressionNode::ReductionContext reductionContext = Poincare::ExpressionNode::ReductionContext(context, 
          preferences->complexFormat(), preferences->angleUnit(), 
          GlobalPreferences::sharedGlobalPreferences()->unitFormat(), 
          ExpressionNode::ReductionTarget::SystemForApproximation, 
          ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, 
          Poincare::ExpressionNode::UnitConversion::Default);

  PoincareHelpers::Reduce(&m_expression, context, ExpressionNode::ReductionTarget::SystemForAnalysis);

  int degree = m_expression.getPolynomialReducedCoefficients(
          "x",
          polynomialCoefficients,
          context,
          Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), m_expression, context),
          preferences->angleUnit(),
          GlobalPreferences::sharedGlobalPreferences()->unitFormat(),
          ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);

  assert(degree == 2);

  Expression a = polynomialCoefficients[2];
  Expression b = polynomialCoefficients[1];
  Expression c = polynomialCoefficients[0];
  
  Expression delta = Subtraction::Builder(Power::Builder(b.clone(), Rational::Builder(2)), Multiplication::Builder(Rational::Builder(4), a.clone(), c.clone()));
  PoincareHelpers::Simplify(&delta, context, ExpressionNode::ReductionTarget::SystemForApproximation);

  // Alpha is -b/2a, but because after we use -α, we immediately store -α=-(-b/2a)=b/2a.
  Expression minusAlpha = Division::Builder(b.clone(), Multiplication::Builder(Rational::Builder(2), a.clone()));
  PoincareHelpers::Reduce(&minusAlpha, context, ExpressionNode::ReductionTarget::SystemForApproximation);

  // Same thing for β
  Expression minusBeta = Division::Builder(delta.clone(), Multiplication::Builder(Rational::Builder(4), a.clone()));
  PoincareHelpers::Reduce(&minusBeta, context, ExpressionNode::ReductionTarget::SystemForApproximation);

  enum MultiplicationTypeForA {
    Nothing,
    Minus,
    Parenthesis,
    Normal
  };

  MultiplicationTypeForA multiplicationTypeForA;
  
  if (a.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(a).isOne()) {
    multiplicationTypeForA = MultiplicationTypeForA::Nothing;
  }
  else if(a.type() == ExpressionNode::Type::Rational &&  static_cast<const Rational &>(a).isMinusOne()){
    multiplicationTypeForA = MultiplicationTypeForA::Minus;
  }
  else if (a.type() == ExpressionNode::Type::Addition) {
    multiplicationTypeForA = MultiplicationTypeForA::Parenthesis;
  }
  else {
    multiplicationTypeForA = MultiplicationTypeForA::Normal;
  }

  PoincareHelpers::Simplify(&a, context, ExpressionNode::ReductionTarget::User);

  /* 
   * Because when can't apply reduce or simplify to keep the 
   * canonized form we must beautify the expression manually 
  */

  Expression xMinusAlphaPowerTwo;
  Expression alpha = getOppositeIfExists(minusAlpha, &reductionContext);

  if (alpha.isUninitialized()) {
    PoincareHelpers::Simplify(&minusAlpha, context, ExpressionNode::ReductionTarget::User);
    xMinusAlphaPowerTwo = Power::Builder(Parenthesis::Builder(Addition::Builder(Symbol::Builder("x", strlen("x")), minusAlpha)), Rational::Builder(2));
  }
  else {
    PoincareHelpers::Simplify(&alpha, context, ExpressionNode::ReductionTarget::User);
    xMinusAlphaPowerTwo = Power::Builder(Parenthesis::Builder(Subtraction::Builder(Symbol::Builder("x", strlen("x")), alpha)), Rational::Builder(2));
  }
  
  Expression xMinusAlphaPowerTwoWithFactor;

  switch (multiplicationTypeForA)
  {
    case MultiplicationTypeForA::Nothing:
      xMinusAlphaPowerTwoWithFactor = xMinusAlphaPowerTwo;
      break;
    case MultiplicationTypeForA::Minus:
      xMinusAlphaPowerTwoWithFactor = Multiplication::Builder(a.clone(), xMinusAlphaPowerTwo);
      break;
    case MultiplicationTypeForA::Parenthesis:
      xMinusAlphaPowerTwoWithFactor = Multiplication::Builder(Parenthesis::Builder(a.clone()), xMinusAlphaPowerTwo);
      break;
    case MultiplicationTypeForA::Normal:
      xMinusAlphaPowerTwoWithFactor = Multiplication::Builder(a.clone(), xMinusAlphaPowerTwo);
      break;
    default:
      assert(false);
      break;
  }
  
  Expression canonized;
  Expression beta = getOppositeIfExists(minusBeta, &reductionContext);
  if (beta.isUninitialized()) {
    PoincareHelpers::Simplify(&minusBeta, context, ExpressionNode::ReductionTarget::User);
    canonized = Subtraction::Builder(xMinusAlphaPowerTwoWithFactor, minusBeta);
  }
  else {
    PoincareHelpers::Simplify(&beta, context, ExpressionNode::ReductionTarget::User);
    canonized = Addition::Builder(xMinusAlphaPowerTwoWithFactor, beta);
  }
 
  Expression x0;
  Expression x1;

  if (delta.nullStatus(context) == ExpressionNode::NullStatus::Null) {
    // x0 = x1 = -b/(2a)
    x0 = Division::Builder(Opposite::Builder(b.clone()), Multiplication::Builder(Rational::Builder(2), a.clone()));
    m_numberOfSolutions = 1;
    PoincareHelpers::Simplify(&x0, context, ExpressionNode::ReductionTarget::SystemForApproximation);
  } 
  else {
    // x0 = (-b-sqrt(delta))/(2a)
    x0 = Division::Builder(Subtraction::Builder(Opposite::Builder(b.clone()), SquareRoot::Builder(delta.clone())), Multiplication::Builder(Rational::Builder(2), a.clone()));
    // x1 = (-b+sqrt(delta))/(2a)
    x1 = Division::Builder(Addition::Builder(Opposite::Builder(b.clone()), SquareRoot::Builder(delta.clone())), Multiplication::Builder(Rational::Builder(2), a.clone()));
    m_numberOfSolutions = 2;
    PoincareHelpers::Simplify(&x0, context, ExpressionNode::ReductionTarget::SystemForApproximation);
    PoincareHelpers::Simplify(&x1, context, ExpressionNode::ReductionTarget::SystemForApproximation);
    if (x0.type() == ExpressionNode::Type::Unreal) {
      assert(x1.type() == ExpressionNode::Type::Unreal);
      m_numberOfSolutions = 0;
    }
  }

  Expression factorized;

  if (m_numberOfSolutions == 2) {
    Expression firstFactor;
    Expression secondFactor;

    Expression x0Opposite = getOppositeIfExists(x0, &reductionContext);
    if (x0Opposite.isUninitialized()) {
      PoincareHelpers::Simplify(&x0, context, ExpressionNode::ReductionTarget::User);
      if (x0.type() == ExpressionNode::Type::Addition || x0.type() == ExpressionNode::Type::Subtraction) {
        x0 = Parenthesis::Builder(x0.clone());
      }
      firstFactor = Subtraction::Builder(Symbol::Builder("x", strlen("x")), x0);
    }
    else {
      PoincareHelpers::Simplify(&x0Opposite, context, ExpressionNode::ReductionTarget::User);
      if (x0Opposite.type() == ExpressionNode::Type::Addition || x0Opposite.type() == ExpressionNode::Type::Subtraction) {
        x0Opposite = Parenthesis::Builder(x0Opposite.clone());
      }
      firstFactor = Addition::Builder(Symbol::Builder("x", strlen("x")), x0Opposite);
    }
    if (x0.type() == ExpressionNode::Type::Opposite) {
      factorized = Parenthesis::Builder(Addition::Builder(Symbol::Builder("x", strlen("x")), x0.childAtIndex(0).clone()));
    }

    Expression x1Opposite = getOppositeIfExists(x1, &reductionContext);
    if (x1Opposite.isUninitialized()) {
      PoincareHelpers::Simplify(&x1, context, ExpressionNode::ReductionTarget::User);
      if (x1.type() == ExpressionNode::Type::Addition || x1.type() == ExpressionNode::Type::Subtraction) {
        x1 = Parenthesis::Builder(x1.clone());
      }
      secondFactor = Subtraction::Builder(Symbol::Builder("x", strlen("x")), x1);
    }
    else {
      PoincareHelpers::Simplify(&x1Opposite, context, ExpressionNode::ReductionTarget::User);
      if (x1Opposite.type() == ExpressionNode::Type::Addition || x1Opposite.type() == ExpressionNode::Type::Subtraction) {
        x1Opposite = Parenthesis::Builder(x1Opposite.clone());
      }
      secondFactor = Addition::Builder(Symbol::Builder("x", strlen("x")), x1Opposite);
    }

    Expression solutionProduct = Multiplication::Builder(Parenthesis::Builder(firstFactor), Parenthesis::Builder(secondFactor));
    switch (multiplicationTypeForA)
    {
      case MultiplicationTypeForA::Nothing:
        factorized = solutionProduct;
        break;
      case MultiplicationTypeForA::Minus:
        factorized = Multiplication::Builder(a.clone(), solutionProduct);
        break;
      case MultiplicationTypeForA::Parenthesis:
        factorized = Multiplication::Builder(Parenthesis::Builder(a.clone()), solutionProduct);
        break;
      case MultiplicationTypeForA::Normal:
        factorized = Multiplication::Builder(a.clone(), solutionProduct);
        break;
      default:
        assert(false);
        break;
    }
  }
  else if (m_numberOfSolutions == 1) {
    Expression x0Opposite = getOppositeIfExists(x0, &reductionContext);
    Expression factor;
    
    if (x0Opposite.isUninitialized()) {
      PoincareHelpers::Simplify(&x0, context, ExpressionNode::ReductionTarget::User);
      factor = Subtraction::Builder(Symbol::Builder("x", strlen("x")), x0);
    }
    else {
      PoincareHelpers::Simplify(&x0Opposite, context, ExpressionNode::ReductionTarget::User);
      factor = Addition::Builder(Symbol::Builder("x", strlen("x")), x0Opposite);
    }

    Expression solutionProduct = Power::Builder(Parenthesis::Builder(factor), Rational::Builder(2));
    switch (multiplicationTypeForA)
    {
      case MultiplicationTypeForA::Nothing:
        factorized = solutionProduct;
        break;
      case MultiplicationTypeForA::Minus:
        factorized = Multiplication::Builder(a.clone(), solutionProduct);
        break;
      case MultiplicationTypeForA::Parenthesis:
        factorized = Multiplication::Builder(Parenthesis::Builder(a.clone()), solutionProduct);
        break;
      case MultiplicationTypeForA::Normal:
        factorized = Multiplication::Builder(a.clone(), solutionProduct);
        break;
      default:
        assert(false);
        break;
    }
  }
  
  PoincareHelpers::Simplify(&delta, context, ExpressionNode::ReductionTarget::User);

  m_layouts[0] = PoincareHelpers::CreateLayout(canonized);
  if (m_numberOfSolutions > 0) {
    m_layouts[1] = PoincareHelpers::CreateLayout(factorized);
    m_layouts[2] = PoincareHelpers::CreateLayout(delta);
    m_layouts[3] = PoincareHelpers::CreateLayout(x0);
    if (m_numberOfSolutions > 1) {
      m_layouts[4] = PoincareHelpers::CreateLayout(x1);
    }
  }
  else {
    m_layouts[1] = PoincareHelpers::CreateLayout(delta);
  }
}

Expression SecondDegreeListController::getOppositeIfExists(Expression e, Poincare::ExpressionNode::ReductionContext * reductionContext) {
  if (e.isNumber() && e.sign(reductionContext->context()) == ExpressionNode::Sign::Negative) {
    Number n = static_cast<Number&>(e);
    return std::move(n.setSign(ExpressionNode::Sign::Positive));
  }
  else if(e.type() == ExpressionNode::Type::Opposite) {
    return std::move(e.childAtIndex(0).clone());
  }
  else if (e.type() == ExpressionNode::Type::Multiplication && e.numberOfChildren() > 0 && e.childAtIndex(0).isNumber() && e.childAtIndex(0).sign(reductionContext->context()) == ExpressionNode::Sign::Negative) {
    Multiplication m = static_cast<Multiplication&>(e);
    if (m.childAtIndex(0).type() == ExpressionNode::Type::Rational && static_cast<Rational&>(e).isMinusOne()) {
      // The negative numeral factor is -1, we just remove it
      m.removeChildAtIndexInPlace(0);
    } else {
      Expression firstChild = m.childAtIndex(0);
      Number n = static_cast<Number&>(firstChild);
      m.childAtIndex(0).setChildrenInPlace(n.setSign(ExpressionNode::Sign::Positive));
    }
    PoincareHelpers::Simplify(&m, reductionContext->context(), ExpressionNode::ReductionTarget::User);
    return std::move(m);
  }
  return Expression();
}

I18n::Message SecondDegreeListController::messageAtIndex(int index) {
  if (m_numberOfSolutions > 0) {
    if (index == 0) {
      return I18n::Message::CanonicalForm;
    } 
    if (index == 1) {
      return I18n::Message::FactorizedForm;
    }
    if (index == 2) {
      return I18n::Message::Discriminant;
    }
    if (index == 3) {
      if (m_numberOfSolutions == 1) {
        return I18n::Message::OnlyRoot;
      }
      else {
        return I18n::Message::FirstRoot;
      }
    }
    return I18n::Message::SecondRoot;
  }
  else {
    switch (index) {
      case 0:
        return I18n::Message::CanonicalForm;
      default:
        return I18n::Message::Discriminant;
    }
  }
}

}
