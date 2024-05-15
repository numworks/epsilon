#include "equation.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/utf8_helper.h>
#include <poincare/old/boolean.h>
#include <poincare/old/comparison.h>
#include <poincare/old/constant.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/nonreal.h>
#include <poincare/old/rational.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/undefined.h>

using namespace Ion;
using namespace Poincare;
using namespace Shared;

namespace Solver {

bool Equation::containsIComplex(Context *context,
                                SymbolicComputation replaceSymbols) const {
  return expressionClone().hasComplexI(context, replaceSymbols);
}

SystemExpression Equation::Model::standardForm(
    const Storage::Record *record, Context *context,
    bool replaceFunctionsButNotSymbols, ReductionTarget reductionTarget) const {
  SystemExpression returnedExpression = SystemExpression();
  // In any case, undefined symbols must be preserved.
  SymbolicComputation symbolicComputation =
      replaceFunctionsButNotSymbols
          ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
          : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  UserExpression expressionInputWithoutFunctions =
      NewExpression::ExpressionWithoutSymbols(expressionClone(record), context,
                                              symbolicComputation);
  if (expressionInputWithoutFunctions.isUninitialized()) {
    // The expression is circularly-defined
    expressionInputWithoutFunctions = Undefined::Builder();
  }
  EmptyContext emptyContext;
  Context *contextToUse =
      replaceFunctionsButNotSymbols ? &emptyContext : context;

  // Reduce the expression
  UserExpression simplifiedInput = expressionInputWithoutFunctions;
  PoincareHelpers::CloneAndSimplify(&simplifiedInput, contextToUse,
                                    {.target = reductionTarget});

  if (simplifiedInput.type() == ExpressionNode::Type::Nonreal) {
    returnedExpression = Nonreal::Builder();
  } else if (simplifiedInput.recursivelyMatches(
                 [](const NewExpression e, Context *context) {
                   return e.isOfType({ExpressionNode::Type::Undefined,
                                      ExpressionNode::Type::Infinity}) ||
                          NewExpression::IsMatrix(e, context);
                 },
                 contextToUse)) {
    returnedExpression = Undefined::Builder();
  } else if (ComparisonNode::IsBinaryEquality(simplifiedInput)) {
    returnedExpression = Subtraction::Builder(simplifiedInput.childAtIndex(0),
                                              simplifiedInput.childAtIndex(1));
    ReductionContext reductionContext =
        PoincareHelpers::ReductionContextForParameters(
            expressionInputWithoutFunctions, contextToUse,
            {.target = reductionTarget});
    returnedExpression = returnedExpression.cloneAndReduce(reductionContext);
  } else {
    assert(simplifiedInput.isOfType(
        {ExpressionNode::Type::Boolean, ExpressionNode::Type::List}));
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
