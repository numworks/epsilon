#include "equation.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/boolean.h>
#include <poincare/comparison.h>
#include <poincare/empty_context.h>
#include <poincare/nonreal.h>
#include <poincare/rational.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

using namespace Ion;
using namespace Shared;

namespace Solver {

bool Equation::containsIComplex(
    Poincare::Context *context,
    Poincare::SymbolicComputation replaceSymbols) const {
  return expressionClone().hasComplexI(context, replaceSymbols);
}

Poincare::Expression Equation::Model::standardForm(
    const Storage::Record *record, Poincare::Context *context,
    bool replaceFunctionsButNotSymbols,
    Poincare::ReductionTarget reductionTarget) const {
  Poincare::Expression returnedExpression = Poincare::Expression();
  // In any case, undefined symbols must be preserved.
  Poincare::SymbolicComputation symbolicComputation =
      replaceFunctionsButNotSymbols
          ? Poincare::SymbolicComputation::
                ReplaceDefinedFunctionsWithDefinitions
          : Poincare::SymbolicComputation::
                ReplaceAllDefinedSymbolsWithDefinition;
  Poincare::Expression expressionInputWithoutFunctions =
      Poincare::Expression::ExpressionWithoutSymbols(
          expressionClone(record), context, symbolicComputation);
  if (expressionInputWithoutFunctions.isUninitialized()) {
    // The expression is circularly-defined
    expressionInputWithoutFunctions = Poincare::Undefined::Builder();
  }
  Poincare::EmptyContext emptyContext;
  Poincare::Context *contextToUse =
      replaceFunctionsButNotSymbols ? &emptyContext : context;

  // Reduce the expression
  Poincare::Expression simplifiedInput = expressionInputWithoutFunctions;
  PoincareHelpers::CloneAndSimplify(&simplifiedInput, contextToUse,
                                    {.target = reductionTarget});

  if (simplifiedInput.type() == Poincare::ExpressionNode::Type::Nonreal) {
    returnedExpression = Poincare::Nonreal::Builder();
  } else if (simplifiedInput.recursivelyMatches(
                 [](const Poincare::Expression e, Poincare::Context *context) {
                   return e.isOfType(
                              {Poincare::ExpressionNode::Type::Undefined,
                               Poincare::ExpressionNode::Type::Infinity}) ||
                          Poincare::Expression::IsMatrix(e, context);
                 },
                 contextToUse)) {
    returnedExpression = Poincare::Undefined::Builder();
  } else if (Poincare::ComparisonNode::IsBinaryEquality(simplifiedInput)) {
    returnedExpression = Poincare::Subtraction::Builder(
        simplifiedInput.childAtIndex(0), simplifiedInput.childAtIndex(1));
    Poincare::ReductionContext reductionContext =
        PoincareHelpers::ReductionContextForParameters(
            expressionInputWithoutFunctions, contextToUse,
            {.target = reductionTarget});
    returnedExpression = returnedExpression.cloneAndReduce(reductionContext);
  } else {
    assert(simplifiedInput.isOfType({Poincare::ExpressionNode::Type::Boolean,
                                     Poincare::ExpressionNode::Type::List}));
    /* The equality has disappeared after reduction. This may be because:
     * - the comparison was always true or false (e.g. 1 = 0) and has been
     *   reduced to a boolean.
     * - the equal sign has been distributed inside a list
     * Return 1 if the equation has no solution (since it is equivalent to
     * 1 = 0) or 0 if it has infinite solutions. */
    returnedExpression =
        simplifiedInput.type() == Poincare::ExpressionNode::Type::Boolean &&
                static_cast<Poincare::Boolean &>(simplifiedInput).value()
            ? Poincare::Rational::Builder(0)
            : Poincare::Rational::Builder(1);
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
