#include <poincare/sum_and_product.h>
#include <poincare/decimal.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>
#include <poincare/integer_variable_context.h>
#include <poincare/sum.h>
#include <poincare/product.h>
#include <poincare/symbol.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

Layout SumAndProductNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return createSumAndProductLayout(
    childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
    childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits),
    childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits),
    childAtIndex(3)->createLayout(floatDisplayMode, numberOfSignificantDigits)
  );
}

Expression SumAndProductNode::shallowReduce(ReductionContext reductionContext) {
  return SumAndProduct(this).shallowReduce(reductionContext.context());
}

template<typename T>
Evaluation<T> SumAndProductNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> aInput = childAtIndex(2)->approximate(T(), approximationContext);
  Evaluation<T> bInput = childAtIndex(3)->approximate(T(), approximationContext);
  T start = aInput.toScalar();
  T end = bInput.toScalar();
  if (std::isnan(start) || std::isnan(end) || start != (int)start || end != (int)end || end - start > k_maxNumberOfSteps) {
    return Complex<T>::Undefined();
  }
  SymbolNode * symbol = static_cast<SymbolNode *>(childAtIndex(1));
  VariableContext nContext = VariableContext(symbol->name(), approximationContext.context());
  Evaluation<T> result = Complex<T>::Builder((T)emptySumAndProductValue());
  for (int i = (int)start; i <= (int)end; i++) {
    if (Expression::ShouldStopProcessing()) {
      return Complex<T>::Undefined();
    }
    nContext.setApproximationForVariable<T>((T)i);
    Expression child = Expression(childAtIndex(0)).clone();
    approximationContext.setContext(&nContext);
    result = evaluateWithNextTerm(T(), result, child.node()->approximate(T(), approximationContext), approximationContext.complexFormat());
    if (result.isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return result;
}

Expression SumAndProductNode::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  return SumAndProduct(this).deepReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount);
}

Expression SumAndProduct::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  int nbChildren = numberOfChildren();
  for (int i = 1; i < nbChildren; i++) {
    Expression c = childAtIndex(i).deepReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount);
    if (c.isUninitialized()) {
      return Expression();
    }
  }

  Symbol symbol = childAtIndex(1).convert<Symbol>();
  IntegerVariableContext newContext = IntegerVariableContext(symbol.name(), context);
  Expression c = childAtIndex(0).deepReplaceReplaceableSymbols(&newContext, didReplace, replaceFunctionsOnly, parameteredAncestorsCount);
  if (c.isUninitialized()) {
    return Expression();
  }

  return *this;
}

void SumAndProductNode::deepReduceChildren(ReductionContext reductionContext) {
  SymbolNode * symbol = static_cast<SymbolNode *>(childAtIndex(1));
  IntegerVariableContext newContext = IntegerVariableContext(symbol->name(), reductionContext.context());
  reductionContext.setContext(&newContext);
  ExpressionNode::deepReduceChildren(reductionContext);
}

Expression SumAndProduct::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  assert(!childAtIndex(1).deepIsMatrix(context));
  if (childAtIndex(2).deepIsMatrix(context) || childAtIndex(3).deepIsMatrix(context)) {
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

template Evaluation<float> SumAndProductNode::templatedApproximate(ApproximationContext approximationContext) const;
template Evaluation<double> SumAndProductNode::templatedApproximate(ApproximationContext approximationContext) const;

}
