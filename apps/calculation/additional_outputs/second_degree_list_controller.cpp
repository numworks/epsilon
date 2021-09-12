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

  bool aIsNotOne = !(a.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(a).isOne());

  Expression delta = Subtraction::Builder(Power::Builder(b.clone(), Rational::Builder(2)), Multiplication::Builder(Rational::Builder(4), a.clone(), c.clone()));
  PoincareHelpers::Simplify(&delta, context, ExpressionNode::ReductionTarget::SystemForApproximation);

  Expression alpha = Opposite::Builder(Division::Builder(b.clone(), Multiplication::Builder(Rational::Builder(2), a.clone())));
  PoincareHelpers::Simplify(&alpha, context, ExpressionNode::ReductionTarget::User);

  Expression beta = Opposite::Builder(Division::Builder(delta.clone(), Multiplication::Builder(Rational::Builder(4), a.clone())));
  PoincareHelpers::Simplify(&beta, context, ExpressionNode::ReductionTarget::User);

  /* 
   * Because when can't apply reduce or simplify to keep the canonised 
   * we must beautify the expression manually 
  */

  Expression canonised;
  if (alpha.type() == ExpressionNode::Type::Opposite) {
    canonised = Addition::Builder(Symbol::Builder("x", strlen("x")), alpha.childAtIndex(0).clone());
  }
  else {
    canonised = Subtraction::Builder(Symbol::Builder("x", strlen("x")), alpha.clone());
  }
  canonised = Power::Builder(Parenthesis::Builder(canonised.clone()), Rational::Builder(2));
  if (aIsNotOne) {
    canonised = Multiplication::Builder(a.clone(), canonised.clone());
  }
  if (beta.type() == ExpressionNode::Type::Opposite) {
    canonised = Subtraction::Builder(canonised.clone(), beta.childAtIndex(0).clone());
  }
  else {
    canonised = Addition::Builder(canonised.clone(), beta.clone());
  }

 
  Expression x0;
  Expression x1;


  if (delta.nullStatus(context) == ExpressionNode::NullStatus::Null) {
    // x0 = x1 = -b/(2a)
    x0 = Division::Builder(Opposite::Builder(b), Multiplication::Builder(Rational::Builder(2), a));
    m_numberOfSolutions = 1;
    PoincareHelpers::Simplify(&x0, context, ExpressionNode::ReductionTarget::User);
  } 
  else {
    // x0 = (-b-sqrt(delta))/(2a)
    x0 = Division::Builder(Subtraction::Builder(Opposite::Builder(b.clone()), SquareRoot::Builder(delta.clone())), Multiplication::Builder(Rational::Builder(2), a.clone()));
    // x1 = (-b+sqrt(delta))/(2a)
    x1 = Division::Builder(Addition::Builder(Opposite::Builder(b), SquareRoot::Builder(delta.clone())), Multiplication::Builder(Rational::Builder(2), a));
    m_numberOfSolutions = 2;
    PoincareHelpers::Simplify(&x0, context, ExpressionNode::ReductionTarget::User);
    PoincareHelpers::Simplify(&x1, context, ExpressionNode::ReductionTarget::User);
    if (x0.type() == ExpressionNode::Type::Unreal) {
      assert(x1.type() == ExpressionNode::Type::Unreal);
      m_numberOfSolutions = 0;
    }
  }

  Expression factorized;

  if (m_numberOfSolutions == 2) {
    if (x0.type() == ExpressionNode::Type::Opposite) {
      factorized = Parenthesis::Builder(Addition::Builder(Symbol::Builder("x", strlen("x")), x0.childAtIndex(0).clone()));
    }
    else {
      factorized = Parenthesis::Builder(Subtraction::Builder(Symbol::Builder("x", strlen("x")), x0.clone()));
    }
    
    if (x1.type() == ExpressionNode::Type::Opposite) {
      factorized = Multiplication::Builder(factorized.clone(), Parenthesis::Builder(Addition::Builder(Symbol::Builder("x", strlen("x")), x1.childAtIndex(0).clone())));
    }
    else {
      factorized = Multiplication::Builder(factorized.clone(), Parenthesis::Builder(Subtraction::Builder(Symbol::Builder("x", strlen("x")), x1.clone())));
    }

    if (aIsNotOne) {
      factorized = Multiplication::Builder(a.clone(), factorized.clone());
    }
  }
  else if (m_numberOfSolutions == 1) {
    if (x0.type() == ExpressionNode::Type::Opposite) {
      factorized = Power::Builder(Parenthesis::Builder(Addition::Builder(Symbol::Builder("x", strlen("x")), x0.childAtIndex(0).clone())), Rational::Builder(2));
    }
    else {
      factorized = Power::Builder(Parenthesis::Builder(Subtraction::Builder(Symbol::Builder("x", strlen("x")), x0.clone())), Rational::Builder(2));
    }

    if (aIsNotOne) {
      factorized = Multiplication::Builder(a.clone(), factorized.clone());
    }
  }
  
  PoincareHelpers::Simplify(&delta, context, ExpressionNode::ReductionTarget::User);

  m_layouts[0] = PoincareHelpers::CreateLayout(canonised);
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
