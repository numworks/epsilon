#include "equation.h"
#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/boolean.h>
#include <poincare/constant.h>
#include <poincare/comparison.h>
#include <poincare/empty_context.h>
#include <poincare/undefined.h>
#include <poincare/nonreal.h>
#include <poincare/rational.h>
#include <poincare/subtraction.h>
#include <ion/unicode/utf8_helper.h>

using namespace Ion;
using namespace Poincare;
using namespace Shared;

namespace Solver {

bool Equation::containsIComplex(Context * context, ExpressionNode::SymbolicComputation replaceSymbols) const {
  return expressionClone().hasComplexI(context, replaceSymbols);
}

Expression Equation::Model::standardForm(const Storage::Record * record, Context * context, bool replaceFunctionsButNotSymbols, ExpressionNode::ReductionTarget reductionTarget) const {
  Expression returnedExpression = Expression();
  // In any case, undefined symbols must be preserved.
  ExpressionNode::SymbolicComputation symbolicComputation = replaceFunctionsButNotSymbols ? ExpressionNode::SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions : ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  Expression expressionInputWithoutFunctions = Expression::ExpressionWithoutSymbols(expressionClone(record), context, symbolicComputation);
  if (expressionInputWithoutFunctions.isUninitialized()) {
    // The expression is circularly-defined
    expressionInputWithoutFunctions = Undefined::Builder();
  }
  EmptyContext emptyContext;
  Context * contextToUse = replaceFunctionsButNotSymbols ? &emptyContext : context;

  // Reduce the expression
  Expression expressionRed = expressionInputWithoutFunctions;
  PoincareHelpers::CloneAndSimplify(&expressionRed, contextToUse, reductionTarget);

  if (expressionRed.type() == ExpressionNode::Type::Nonreal) {
    returnedExpression = Nonreal::Builder();
  } else if (expressionRed.recursivelyMatches(
        [](const Expression e, Context * context) {
          return e.type() == ExpressionNode::Type::Undefined || e.type() == ExpressionNode::Type::Infinity || Expression::IsMatrix(e, context);
        },
        contextToUse))
  {
    returnedExpression = Undefined::Builder();
  } else if (ComparisonNode::IsBinaryEquality(expressionRed)) {
    Preferences * preferences = Preferences::sharedPreferences();
    returnedExpression = Subtraction::Builder(expressionRed.childAtIndex(0), expressionRed.childAtIndex(1));
    returnedExpression = returnedExpression.cloneAndReduce(ExpressionNode::ReductionContext(contextToUse, Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), expressionInputWithoutFunctions, contextToUse), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), reductionTarget));
  } else {
    assert(expressionRed.type() == ExpressionNode::Type::Boolean);
    /* The equality was reduced which means the equality was either
     * always true or always false.
     * Return 1 if it's false (since it's equivalent to the equation 1 = 0),
     * and return 0 if it's true (since it's equivalent to 0 = 0). */
    returnedExpression = static_cast<const Boolean&>(expressionRed).value() ? Rational::Builder(0) : Rational::Builder(1);
  }
  return returnedExpression;
}

void * Equation::Model::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer;
}

size_t Equation::Model::expressionSize(const Ion::Storage::Record * record) const {
  return record->value().size;
}

}
