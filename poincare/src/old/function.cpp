#include <poincare/layout.h>
#include <poincare/old/dependency.h>
#include <poincare/old/function.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>
#include <poincare/src/memory/tree_stack.h>

#include <cmath>

namespace Poincare {

/* Usual behavior for functions is to expand itself (as well as any function it
 * contains), before calling the same method on its definition (or handle it if
 * uninitialized). We do this in getPolynomialCoefficients, getVariables,
 * templatedApproximate and shallowReduce. */

int FunctionNode::getPolynomialCoefficients(Context* context,
                                            const char* symbolName,
                                            OExpression coefficients[]) const {
  Function f(this);
  OExpression e = SymbolAbstract::Expand(
      f, context, true, SymbolicComputation::ReplaceDefinedFunctions);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.getPolynomialCoefficients(context, symbolName, coefficients);
}

int FunctionNode::getVariables(Context* context, isVariableTest isVariable,
                               char* variables, int maxSizeVariable,
                               int nextVariableIndex) const {
  Function f(this);
  OExpression e = SymbolAbstract::Expand(
      f, context, true, SymbolicComputation::ReplaceDefinedFunctions);
  if (e.isUninitialized()) {
    return nextVariableIndex;
  }
  return e.node()->getVariables(context, isVariable, variables, maxSizeVariable,
                                nextVariableIndex);
}

size_t FunctionNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode,
                                     numberOfSignificantDigits, m_name);
}

OExpression FunctionNode::shallowReduce(
    const ReductionContext& reductionContext) {
  // This uses Symbol::shallowReduce
  return Function(this).shallowReduce(reductionContext);
}

Evaluation<float> FunctionNode::approximate(
    SinglePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<float>(approximationContext);
}

Evaluation<double> FunctionNode::approximate(
    DoublePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<double>(approximationContext);
}

template <typename T>
Evaluation<T> FunctionNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Function f(this);
  OExpression e =
      SymbolAbstract::Expand(f, approximationContext.context(), true,
                             SymbolicComputation::ReplaceDefinedFunctions);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), approximationContext);
}

OExpression Function::shallowReduce(ReductionContext reductionContext) {
  SymbolicComputation symbolicComputation =
      reductionContext.symbolicComputation();
  if (symbolicComputation ==
      SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    return replaceWithUndefinedInPlace();
  }

  // Bubble up dependencies of children
  OExpression e =
      SimplificationHelper::bubbleUpDependencies(*this, reductionContext);
  if (!e.isUninitialized()) {
    return e;
  }

  if (symbolicComputation == SymbolicComputation::KeepAllSymbols) {
    return *this;
  }

  assert(symbolicComputation == SymbolicComputation::ReplaceAllSymbols ||
         symbolicComputation == SymbolicComputation::ReplaceDefinedSymbols ||
         symbolicComputation == SymbolicComputation::ReplaceDefinedFunctions);

  /* Symbols that have a definition while also being the parameter of a
   * parametered expression should not be replaced in SymbolAbstract::Expand,
   * which won't handle this expression's parents.
   * With ReplaceDefinedFunctions symbolic computation, only
   * nested functions will be replaced by their definitions.
   * Symbols will be handled in deepReduce, which is aware of parametered
   * expressions context. For example, with 1->x and 1+x->f(x), f(x) within
   * diff(f(x),x,1) should be reduced to 1+x instead of 2. */
  OExpression result =
      SymbolAbstract::Expand(*this, reductionContext.context(), true,
                             SymbolicComputation::ReplaceDefinedFunctions);
  if (result.isUninitialized()) {
    if (symbolicComputation == SymbolicComputation::ReplaceDefinedSymbols ||
        symbolicComputation == SymbolicComputation::ReplaceDefinedFunctions) {
      return *this;
    }
    assert(symbolicComputation == SymbolicComputation::ReplaceAllSymbols);
    return replaceWithUndefinedInPlace();
  }
  replaceWithInPlace(result);
  /* The stored expression is as entered by the user, so we need to call reduce
   * Remaining Nested symbols will be properly expanded as they are reduced. */
  return result.deepReduce(reductionContext);
}

}  // namespace Poincare
