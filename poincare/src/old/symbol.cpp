#include <assert.h>
#include <omg/utf8_decoder.h>
#include <omg/utf8_helper.h>
#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/old/context.h>
#include <poincare/old/parametered_expression.h>
#include <poincare/old/rational.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>
#include <poincare/src/memory/tree_stack.h>
#include <string.h>

#include <cmath>

namespace Poincare {

int SymbolNode::getPolynomialCoefficients(Context* context,
                                          const char* symbolName,
                                          OExpression coefficients[]) const {
  return Symbol(this).getPolynomialCoefficients(context, symbolName,
                                                coefficients);
}

int SymbolNode::getVariables(Context* context, isVariableTest isVariable,
                             char* variables, int maxSizeVariable,
                             int nextVariableIndex) const {
  /* No variable expansion is expected within this method. Only functions are
   * expanded and replaced. */
  if (isVariable(m_name, context)) {
    int index = 0;
    /* variables is in fact of type
     * char[k_maxNumberOfVariables][maxSizeVariable] */
    while (index < maxSizeVariable * OExpression::k_maxNumberOfVariables &&
           variables[index] != 0) {
      if (strcmp(m_name, &variables[index]) == 0) {
        return nextVariableIndex;
      }
      index += maxSizeVariable;
    }
    if (nextVariableIndex < OExpression::k_maxNumberOfVariables) {
      assert(variables[nextVariableIndex * maxSizeVariable] == 0);
      if (strlen(m_name) + 1 > (size_t)maxSizeVariable) {
        return -2;
      }
      strlcpy(&variables[nextVariableIndex * maxSizeVariable], m_name,
              maxSizeVariable);
      nextVariableIndex++;
      if (nextVariableIndex < OExpression::k_maxNumberOfVariables) {
        variables[nextVariableIndex * maxSizeVariable] = 0;
      }
      return nextVariableIndex;
    }
    return -1;
  }
  return nextVariableIndex;
}

OExpression SymbolNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Symbol(this).shallowReduce(reductionContext);
}

bool SymbolNode::derivate(const ReductionContext& reductionContext,
                          Symbol symbol, OExpression symbolValue) {
  return Symbol(this).derivate(reductionContext, symbol, symbolValue);
}

template <typename T>
Evaluation<T> SymbolNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Symbol s(this);
  // No need to preserve undefined symbols because they will be approximated.
  OExpression e =
      SymbolAbstract::Expand(s, approximationContext.context(), true,
                             SymbolicComputation::ReplaceAllSymbols);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), approximationContext);
}

bool SymbolNode::isSystemSymbol() const {
  bool result = UTF8Helper::CodePointIs(m_name, UCodePointUnknown);
  if (result) {
    assert(m_name[1] == 0);
  }
  return result;
}

OExpression Symbol::shallowReduce(ReductionContext reductionContext) {
  assert(false);
  return replaceWithUndefinedInPlace();
#if 0
  SymbolicComputation symbolicComputation =
      reductionContext.symbolicComputation();
  if (symbolicComputation ==
          SymbolicComputation::ReplaceDefinedFunctions ||
      symbolicComputation == SymbolicComputation::KeepAllSymbols) {
    return *this;
  }
  if (symbolicComputation ==
      SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    return replaceWithUndefinedInPlace();
  }
  assert(symbolicComputation ==
             SymbolicComputation::ReplaceAllSymbols ||
         symbolicComputation ==
             SymbolicComputation::ReplaceDefinedSymbols);
  {
    OExpression current = *this;
    OExpression p = parent();
    while (!p.isUninitialized()) {
      if (p.isParameteredExpression()) {
        int index = p.indexOfChild(current);
        if (index == ParameteredExpression::ParameterChildIndex()) {
          // The symbol is a parametered expression's parameter
          return *this;
        }
        if (index == ParameteredExpression::ParameteredChildIndex() &&
            hasSameNameAs(static_cast<ParameteredExpression&>(p).parameter())) {
          return *this;
        }
      }
      current = p;
      p = current.parent();
    }
  }

  /* Recursively replace symbols and catch circular references involving symbols
   * as well as functions. */
  OExpression result = SymbolAbstract::Expand(*this, reductionContext.context(),
                                              true, symbolicComputation);
  if (result.isUninitialized()) {
    if (symbolicComputation ==
        SymbolicComputation::ReplaceDefinedSymbols) {
      return *this;
    }
    assert(symbolicComputation ==
           SymbolicComputation::ReplaceAllSymbols);
    return replaceWithUndefinedInPlace();
  }
  replaceWithInPlace(result);

  /* At this point, any remaining symbol in result is a parameter of a
   * parametered function nested in this expression, (such as 'diff(x,x,2)' in
   * previous example) and it must be preserved.
   * ReductionContext's SymbolicComputation is altered, enforcing preservation
   * of remaining variables only to save computation that has already been
   * done in SymbolAbstract::Expand, when looking for parametered functions. */
  reductionContext.setSymbolicComputation(
      SymbolicComputation::KeepAllSymbols);
  // The stored expression is as entered by the user, so we need to call reduce
  result = result.deepReduce(reductionContext);
  return result;
#endif
}

bool Symbol::derivate(const ReductionContext& reductionContext, Symbol symbol,
                      OExpression symbolValue) {
  replaceWithInPlace(Rational::Builder(strcmp(name(), symbol.name()) == 0));
  return true;
}

int Symbol::getPolynomialCoefficients(Context* context, const char* symbolName,
                                      OExpression coefficients[]) const {
  int deg = polynomialDegree(symbolName);
  if (deg == 1) {
    coefficients[0] = Rational::Builder(0);
    coefficients[1] = Rational::Builder(1);
  } else {
    assert(deg == 0);
    coefficients[0] = clone();
  }
  return deg;
}

}  // namespace Poincare
