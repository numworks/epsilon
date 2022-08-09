#include <poincare/function.h>
#include <poincare/dependency.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <cmath>

namespace Poincare {

FunctionNode::FunctionNode(const char * newName, int length) : SymbolAbstractNode() {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

Expression FunctionNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return Function(this).replaceSymbolWithExpression(symbol, expression);
}

/* Usual behavior for functions is to expand itself (as well as any function it
 * contains), before calling the same method on its definition (or handle it if
 * uninitialized). We do this in polynomialDegree, getPolynomialCoefficients,
 * getVariables, templatedApproximate and shallowReduce. */
int FunctionNode::polynomialDegree(Context * context, const char * symbolName) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true, SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.polynomialDegree(context, symbolName);
}

int FunctionNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true, SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.getPolynomialCoefficients(context, symbolName, coefficients);
}

int FunctionNode::getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true, SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
  if (e.isUninitialized()) {
    return nextVariableIndex;
  }
  return e.node()->getVariables(context, isVariable, variables, maxSizeVariable, nextVariableIndex);
}

Layout FunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(Function(this), floatDisplayMode, numberOfSignificantDigits, name(), context);
}

int FunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression FunctionNode::shallowReduce(const ReductionContext& reductionContext) {
  // This uses Symbol::shallowReduce
  return Function(this).shallowReduce(reductionContext);
}

bool FunctionNode::involvesCircularity(Context * context, int maxDepth, const char * * visitedFunctions, int numberOfVisitedFunctions) {
  // Check if this symbol has already been visited.
  for (int i = 0; i < numberOfVisitedFunctions; i++) {
    if (strcmp(name(), visitedFunctions[i]) == 0) {
      return true;
    }
  }

  // Check variable
  if (ExpressionNode::involvesCircularity(context, maxDepth, visitedFunctions, numberOfVisitedFunctions)) {
    return true;
  }

  // Check for circularity in the expression of the function and decrease depth
  maxDepth--;
  if (maxDepth < 0) {
    /* We went too deep into the check and consider the expression to be
     * circular. */
    return true;
  }
  visitedFunctions[numberOfVisitedFunctions] = name();
  numberOfVisitedFunctions++;

  Expression e = context->expressionForSymbolAbstract(SymbolAbstract(this), false);
  if (e.isUninitialized()) {
    return false;
  }
  return e.involvesCircularity(context, maxDepth, visitedFunctions, numberOfVisitedFunctions);
}

Expression FunctionNode::deepReplaceReplaceableSymbols(Context * context, TrinaryBoolean * isCircular, int parameteredAncestorsCount, SymbolicComputation symbolicComputation) {
  return Function(this).deepReplaceReplaceableSymbols(context, isCircular, parameteredAncestorsCount, symbolicComputation);
}

Evaluation<float> FunctionNode::approximate(SinglePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<float>(approximationContext);
}

Evaluation<double> FunctionNode::approximate(DoublePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<double>(approximationContext);
}

template<typename T>
Evaluation<T> FunctionNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, approximationContext.context(), true, SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), approximationContext);
}

Function Function::Builder(const char * name, size_t length, Expression child) {
  Function f = SymbolAbstract::Builder<Function, FunctionNode>(name, length);
  if (!child.isUninitialized()) {
    f.replaceChildAtIndexInPlace(0, child);
  }
  return f;
}

Expression Function::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  // Replace the symbol in the child
  childAtIndex(0).replaceSymbolWithExpression(symbol, expression);
  if (symbol.type() == ExpressionNode::Type::Function && hasSameNameAs(symbol)) {
    Expression value = expression.clone();
    Expression p = parent();
    Expression variable = symbol.childAtIndex(0);
    assert(variable.type() == ExpressionNode::Type::Symbol);
    value = value.replaceSymbolWithExpression(variable.convert<Symbol>(), childAtIndex(0));
    if (!p.isUninitialized() && p.node()->childAtIndexNeedsUserParentheses(value, p.indexOfChild(*this))) {
      value = Parenthesis::Builder(value);
    }
    replaceWithInPlace(value);
    return value;
  }
  return *this;
}

Expression Function::shallowReduce(ReductionContext reductionContext) {
  if (reductionContext.symbolicComputation() == SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    return replaceWithUndefinedInPlace();
  }

  // Bubble up dependencies of children
  Expression e = SimplificationHelper::bubbleUpDependencies(*this, reductionContext);
  if (!e.isUninitialized()) {
    return e;
  }

  if (reductionContext.symbolicComputation() == SymbolicComputation::DoNotReplaceAnySymbol) {
    return *this;
  }
  /* Symbols that have a definition while also being the parameter of a
   * parametered expression should not be replaced in SymbolAbstract::Expand,
   * which won't handle this expression's parents.
   * With ReplaceDefinedFunctionsWithDefinitions symbolic computation, only
   * nested functions will be replaced by their definitions.
   * Symbols will be handled in deepReduce, which is aware of parametered
   * expressions context. For example, with 1->x and 1+x->f(x), f(x) within
   * diff(f(x),x,1) should be reduced to 1+x instead of 2. */
  Expression result = SymbolAbstract::Expand(*this, reductionContext.context(), true, SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions);
  if (result.isUninitialized()) {
    if (reductionContext.symbolicComputation() != SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined) {
      return *this;
    }
    return replaceWithUndefinedInPlace();
  }
  replaceWithInPlace(result);
  /* The stored expression is as entered by the user, so we need to call reduce
   * Remaining Nested symbols will be properly expanded as they are reduced. */
  return result.deepReduce(reductionContext);
}

Expression Function::deepReplaceReplaceableSymbols(Context * context, TrinaryBoolean * isCircular, int parameteredAncestorsCount, SymbolicComputation symbolicComputation) {
  /* These two symbolic computations parameters make no sense in this method.
   * They are therefore not handled. */
  assert(symbolicComputation != SymbolicComputation::ReplaceAllSymbolsWithUndefined
    && symbolicComputation != SymbolicComputation::DoNotReplaceAnySymbol);
  assert(*isCircular != TrinaryBoolean::True);

  /* Check for circularity only when a function is encountered so that
   * it is not uselessly checked each time deepReplaceReplaceableSymbols is
   * called.
   * isCircularFromHere is used so that isCircular is not altered if this is
   * not circular but a sibling of this is circular and was not checked yet. */
  TrinaryBoolean isCircularFromHere = *isCircular;
  if (isCircularFromHere == TrinaryBoolean::Unknown) {
    const char * visitedFunctions[Expression::k_maxSymbolReplacementsCount];
    isCircularFromHere = BinaryToTrinaryBool(involvesCircularity(context, Expression::k_maxSymbolReplacementsCount, visitedFunctions, 0));
  }
  if (isCircularFromHere == TrinaryBoolean::True) {
    *isCircular = isCircularFromHere;
    return *this;
  }
  assert(isCircularFromHere == TrinaryBoolean::False);

  // Replace replaceable symbols in child
  defaultReplaceReplaceableSymbols(context, &isCircularFromHere, parameteredAncestorsCount, symbolicComputation);
  assert(isCircularFromHere == TrinaryBoolean::False);

  Expression e = context->expressionForSymbolAbstract(*this, false);
  /* On undefined function, ReplaceDefinedFunctionsWithDefinitions is equivalent
   * to ReplaceAllDefinedSymbolsWithDefinition, like in shallowReduce. */
  if (e.isUninitialized()) {
    if (symbolicComputation != SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined) {
      return *this;
    }
    return replaceWithUndefinedInPlace();
  }

  // Build dependency to keep track of function's parameter
  Dependency d = Dependency::Builder(e);
  d.addDependency(childAtIndex(0));
  replaceWithInPlace(d);

  e = e.deepReplaceReplaceableSymbols(context, &isCircularFromHere, parameteredAncestorsCount, symbolicComputation);
  return std::move(d);
}

}
