#include "equation.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/utf8_helper.h>

using namespace Ion;
using namespace Poincare;
using namespace Shared;

namespace Solver {

#if 0
SystemExpression Equation::Model::standardForm(
    const Storage::Record* record, Context* context,
    bool replaceFunctionsButNotSymbols, ReductionTarget reductionTarget) const {
  SystemExpression returnedExpression = SystemExpression();
  // In any case, undefined symbols must be preserved.
  SymbolicComputation symbolicComputation =
      replaceFunctionsButNotSymbols
          ? SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
          : SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
  UserExpression expressionInputWithoutFunctions = expressionClone(record);
  expressionInputWithoutFunctions.replaceSymbols(context, symbolicComputation);
  if (expressionInputWithoutFunctions.isUninitialized()) {
    // The expression is circularly-defined
    expressionInputWithoutFunctions = Undefined::Builder();
  }
  EmptyContext emptyContext;
  Context* contextToUse =
      replaceFunctionsButNotSymbols ? &emptyContext : context;

  // Reduce the expr ession
  UserExpression simplifiedInput = expressionInputWithoutFunctions;
  PoincareHelpers::CloneAndSimplify(&simplifiedInput, contextToUse,
                                    {.target = reductionTarget});

  if (simplifiedInput.isNonReal()) {
    returnedExpression = Nonreal::Builder();
  } else if (simplifiedInput.recursivelyMatches(
                 [](const NewExpression e, Context* context) {
                   return (e.isUndefined() || e.isPlusOrMinusInfinity());
                 },
                 contextToUse) ||
             simplifiedInput.dimension().isMatrix()) {
    returnedExpression = Undefined::Builder();
  } else if (ComparisonNode::IsBinaryEquality(simplifiedInput)) {
    returnedExpression =
        Subtraction::Builder(simplifiedInput.cloneChildAtIndex(0),
                             simplifiedInput.cloneChildAtIndex(1));
    ReductionContext reductionContext =
        PoincareHelpers::ReductionContextForParameters(
            expressionInputWithoutFunctions, contextToUse,
            {.target = reductionTarget});
    returnedExpression = returnedExpression.cloneAndReduce(reductionContext);
  } else {
    assert(simplifiedInput.isBoolean() || simplifiedInput.isList());
    /* The equality has disappeared after reduction. This may be because:
     * - the comparison was always true or false (e.g. 1 = 0) and has been
     *   reduced to a boolean.
     * - the equal sign has been distributed inside a list
     * Return 1 if the equation has no solution (since it is equivalent to
     * 1 = 0) or 0 if it has infinite solutions. */
    returnedExpression = simplifiedInput.isBoolean() &&
                                 static_cast<Boolean&>(simplifiedInput).value()
                             ? Rational::Builder(0)
                             : Rational::Builder(1);
  }
  return returnedExpression;
}
#endif

void* Equation::Model::expressionAddress(
    const Ion::Storage::Record* record) const {
  return (char*)record->value().buffer;
}

size_t Equation::Model::expressionSize(
    const Ion::Storage::Record* record) const {
  return record->value().size;
}

}  // namespace Solver
