#include <poincare/dependency.h>
#include <poincare/function.h>
#include <poincare/layout.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare_junior/src/memory/edition_pool.h>

#include <cmath>

namespace Poincare {

/* Usual behavior for functions is to expand itself (as well as any function it
 * contains), before calling the same method on its definition (or handle it if
 * uninitialized). We do this in polynomialDegree, getPolynomialCoefficients,
 * getVariables, templatedApproximate and shallowReduce. */
int FunctionNode::polynomialDegree(Context* context,
                                   const char* symbolName) const {
  Function f(this);
  OExpression e = SymbolAbstract::Expand(
      f, context, true,
      SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.polynomialDegree(context, symbolName);
}

int FunctionNode::getPolynomialCoefficients(Context* context,
                                            const char* symbolName,
                                            OExpression coefficients[]) const {
  Function f(this);
  OExpression e = SymbolAbstract::Expand(
      f, context, true,
      SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
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
      f, context, true,
      SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
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

OExpression FunctionNode::deepReplaceReplaceableSymbols(
    Context* context, TrinaryBoolean* isCircular, int parameteredAncestorsCount,
    SymbolicComputation symbolicComputation) {
  return Function(this).deepReplaceReplaceableSymbols(
      context, isCircular, parameteredAncestorsCount, symbolicComputation);
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
  OExpression e = SymbolAbstract::Expand(
      f, approximationContext.context(), true,
      SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), approximationContext);
}

Function Function::Builder(const char* name, size_t length,
                           JuniorExpression child) {
  if (AliasesLists::k_thetaAliases.contains(name, length)) {
    name = AliasesLists::k_thetaAliases.mainAlias();
    length = strlen(name);
  }
  PoincareJ::Tree* tree =
      PoincareJ::SharedEditionPool->push<PoincareJ::Type::UserFunction>(
          name, length + 1);
  assert(!child.isUninitialized());
  child.tree()->clone();

  JuniorExpression expr = JuniorExpression::Builder(tree);
  return static_cast<Function&>(expr);
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

  if (symbolicComputation == SymbolicComputation::DoNotReplaceAnySymbol) {
    return *this;
  }

  assert(symbolicComputation ==
             SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined ||
         symbolicComputation ==
             SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition ||
         symbolicComputation ==
             SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);

  /* Symbols that have a definition while also being the parameter of a
   * parametered expression should not be replaced in SymbolAbstract::Expand,
   * which won't handle this expression's parents.
   * With ReplaceDefinedFunctionsWithDefinitions symbolic computation, only
   * nested functions will be replaced by their definitions.
   * Symbols will be handled in deepReduce, which is aware of parametered
   * expressions context. For example, with 1->x and 1+x->f(x), f(x) within
   * diff(f(x),x,1) should be reduced to 1+x instead of 2. */
  OExpression result = SymbolAbstract::Expand(
      *this, reductionContext.context(), true,
      SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
  if (result.isUninitialized()) {
    if (symbolicComputation ==
            SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition ||
        symbolicComputation ==
            SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions) {
      return *this;
    }
    assert(symbolicComputation ==
           SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
    return replaceWithUndefinedInPlace();
  }
  replaceWithInPlace(result);
  /* The stored expression is as entered by the user, so we need to call reduce
   * Remaining Nested symbols will be properly expanded as they are reduced. */
  return result.deepReduce(reductionContext);
}

OExpression Function::deepReplaceReplaceableSymbols(
    Context* context, TrinaryBoolean* isCircular, int parameteredAncestorsCount,
    SymbolicComputation symbolicComputation) {
  /* This symbolic computation parameters make no sense in this method.
   * It is therefore not handled. */
  assert(symbolicComputation != SymbolicComputation::DoNotReplaceAnySymbol);

  if (symbolicComputation ==
      SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    return replaceWithUndefinedInPlace();
  }

  assert(symbolicComputation ==
             SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined ||
         symbolicComputation ==
             SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition ||
         symbolicComputation ==
             SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);

  /* Check for circularity only when a symbol/function is encountered so that
   * it is not uselessly checked each time deepReplaceReplaceableSymbols is
   * called.
   * isCircularFromHere is used so that isCircular is not altered if this is
   * not circular but a sibling of this is circular and was not checked yet. */
  TrinaryBoolean isCircularFromHere = *isCircular;
  checkForCircularityIfNeeded(context, &isCircularFromHere);
  if (isCircularFromHere == TrinaryBoolean::True) {
    *isCircular = isCircularFromHere;
    return *this;
  }
  assert(isCircularFromHere == TrinaryBoolean::False);

  // Replace replaceable symbols in child
  defaultReplaceReplaceableSymbols(context, &isCircularFromHere,
                                   parameteredAncestorsCount,
                                   symbolicComputation);
  assert(isCircularFromHere == TrinaryBoolean::False);

  OExpression e = context->expressionForSymbolAbstract(*this, true);
  /* On undefined function, ReplaceDefinedFunctionsWithDefinitions is equivalent
   * to ReplaceAllDefinedSymbolsWithDefinition, like in shallowReduce. */
  if (e.isUninitialized()) {
    if (symbolicComputation ==
            SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition ||
        symbolicComputation ==
            SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions) {
      return *this;
    }
    assert(symbolicComputation ==
           SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
    return replaceWithUndefinedInPlace();
  }

  // Build dependency to keep track of function's parameter
  Dependency d = Dependency::Builder(e);
  d.addDependency(childAtIndex(0));
  replaceWithInPlace(d);

  e = e.deepReplaceReplaceableSymbols(context, &isCircularFromHere,
                                      parameteredAncestorsCount,
                                      symbolicComputation);
  return std::move(d);
}

}  // namespace Poincare
