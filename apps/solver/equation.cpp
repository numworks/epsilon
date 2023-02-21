#include "equation.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/boolean.h>
#include <poincare/comparison.h>
#include <poincare/constant.h>
#include <poincare/empty_context.h>
#include <poincare/nonreal.h>
#include <poincare/rational.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

using namespace Ion;
using namespace Poincare;
using namespace Shared;

namespace Solver {

bool Equation::containsIComplex(Context *context,
                                SymbolicComputation replaceSymbols) const {
  return expressionClone().hasComplexI(context, replaceSymbols);
}

Expression Equation::Model::standardForm(
    const Storage::Record *record, Context *context,
    bool replaceFunctionsButNotSymbols, ReductionTarget reductionTarget) const {
  Expression returnedExpression = Expression();
  // In any case, undefined symbols must be preserved.
  SymbolicComputation symbolicComputation =
      replaceFunctionsButNotSymbols
          ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
          : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  Expression expressionInputWithoutFunctions =
      Expression::ExpressionWithoutSymbols(expressionClone(record), context,
                                           symbolicComputation);
  if (expressionInputWithoutFunctions.isUninitialized()) {
    // The expression is circularly-defined
    expressionInputWithoutFunctions = Undefined::Builder();
  }
  EmptyContext emptyContext;
  Context *contextToUse =
      replaceFunctionsButNotSymbols ? &emptyContext : context;

  // Reduce the expression
  Expression simplifiedInput = expressionInputWithoutFunctions;
  PoincareHelpers::CloneAndSimplify(&simplifiedInput, contextToUse,
                                    reductionTarget);

  if (simplifiedInput.type() == ExpressionNode::Type::Nonreal) {
    returnedExpression = Nonreal::Builder();
  } else if (simplifiedInput.recursivelyMatches(
                 [](const Expression e, Context *context) {
                   return e.type() == ExpressionNode::Type::Undefined ||
                          e.type() == ExpressionNode::Type::Infinity ||
                          Expression::IsMatrix(e, context);
                 },
                 contextToUse)) {
    returnedExpression = Undefined::Builder();
  } else if (ComparisonNode::IsBinaryEquality(simplifiedInput)) {
    Preferences *preferences = Preferences::sharedPreferences;
    returnedExpression = Subtraction::Builder(simplifiedInput.childAtIndex(0),
                                              simplifiedInput.childAtIndex(1));
    returnedExpression = returnedExpression.cloneAndReduce(ReductionContext(
        contextToUse,
        Preferences::UpdatedComplexFormatWithExpressionInput(
            preferences->complexFormat(), expressionInputWithoutFunctions,
            contextToUse),
        preferences->angleUnit(),
        GlobalPreferences::sharedGlobalPreferences->unitFormat(),
        reductionTarget));
  } else {
    assert(simplifiedInput.type() == ExpressionNode::Type::Boolean ||
           simplifiedInput.type() == ExpressionNode::Type::List);
    /* The equality has disappeared after reduction. This may be because:
     * - the comparison was always true or false (e.g. 1 = 0) and has been
     *   reduced to a boolean.
     * - the equal sign has been distributed inside a list
     * Return 1 if the equation has no solution (since it is equivalent to
     * 1 = 0) or 0 if it has infinite solutions. */
    returnedExpression =
        simplifiedInput.type() == ExpressionNode::Type::Boolean &&
                static_cast<Boolean &>(simplifiedInput).value()
            ? Rational::Builder(0)
            : Rational::Builder(1);
  }
  return returnedExpression;
}

void *Equation::Model::expressionAddress(
    const Ion::Storage::Record *record) const {
  return (char *)record->value().buffer;
}

size_t Equation::Model::expressionSize(
    const Ion::Storage::Record *record) const {
  return record->value().size;
}

}  // namespace Solver
