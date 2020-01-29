#include "equation.h"
#include <apps/shared/poincare_helpers.h>
#include <poincare/constant.h>
#include <poincare/empty_context.h>
#include <poincare/equal.h>
#include <poincare/undefined.h>
#include <poincare/unreal.h>
#include <poincare/rational.h>
#include <ion/unicode/utf8_helper.h>

using namespace Ion;
using namespace Poincare;
using namespace Shared;

namespace Solver {

bool Equation::containsIComplex(Context * context) const {
  return expressionClone().recursivelyMatches([](const Expression e, Context * context) { return e.type() == ExpressionNode::Type::Constant && static_cast<const Constant &>(e).isIComplex(); }, context, true);
}

Expression Equation::Model::standardForm(const Storage::Record * record, Context * context, bool replaceFunctionsButNotSymbols) const {
  Expression * returnedExpression = replaceFunctionsButNotSymbols ? &m_standardFormWithReplacedFunctionsButNotSymbols : &m_standardFormWithReplacedFunctionsAndSymbols;
  if (returnedExpression->isUninitialized()) {
    const Expression expressionInputWithoutFunctions = Expression::ExpressionWithoutSymbols(expressionClone(record), context, replaceFunctionsButNotSymbols);

    EmptyContext emptyContext;
    Context * contextToUse = replaceFunctionsButNotSymbols ? &emptyContext : context;

    // Reduce the expression
    Expression expressionRed = expressionInputWithoutFunctions.clone();
    PoincareHelpers::Simplify(&expressionRed, contextToUse, ExpressionNode::ReductionTarget::SystemForApproximation);
    // simplify might return an uninitialized Expression if interrupted
    if (expressionRed.isUninitialized()) {
      expressionRed = expressionInputWithoutFunctions;
    }

    if (expressionRed.type() == ExpressionNode::Type::Unreal) {
      *returnedExpression = Unreal::Builder();
    } else if (expressionRed.recursivelyMatches(
          [](const Expression e, Context * context) {
            return e.type() == ExpressionNode::Type::Undefined || e.type() == ExpressionNode::Type::Infinity || Expression::IsMatrix(e, context);
          },
          contextToUse,
          true))
    {
      *returnedExpression = Undefined::Builder();
    } else if (expressionRed.type() == ExpressionNode::Type::Equal) {
      Preferences * preferences = Preferences::sharedPreferences();
      *returnedExpression = static_cast<const Equal&>(expressionRed).standardEquation(contextToUse, Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), expressionInputWithoutFunctions, contextToUse), preferences->angleUnit());
    } else {
      assert(expressionRed.type() == ExpressionNode::Type::Rational && static_cast<const Rational&>(expressionRed).isOne());
      // The equality was reduced which means the equality was always true.
      *returnedExpression = Rational::Builder(0);
    }
    if (!m_standardFormWithReplacedFunctionsButNotSymbols.isUninitialized() && !m_standardFormWithReplacedFunctionsAndSymbols.isUninitialized()) {
      // Do not keep two equal expressions
      if (m_standardFormWithReplacedFunctionsButNotSymbols.isIdenticalTo(m_standardFormWithReplacedFunctionsAndSymbols)) {
        m_standardFormWithReplacedFunctionsButNotSymbols = m_standardFormWithReplacedFunctionsAndSymbols;
      }
    }
  }
  return *returnedExpression;
}

void Equation::Model::tidy() const {
  ExpressionModel::tidy();
  // Free the pool of the m_standardForm
  m_standardFormWithReplacedFunctionsAndSymbols = Expression();
  m_standardFormWithReplacedFunctionsButNotSymbols = Expression();
}

void * Equation::Model::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer;
}

size_t Equation::Model::expressionSize(const Ion::Storage::Record * record) const {
  return record->value().size;
}

}
