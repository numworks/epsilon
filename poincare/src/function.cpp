#include <poincare/function.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
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

int FunctionNode::polynomialDegree(Context * context, const char * symbolName) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.polynomialDegree(context, symbolName);
}

int FunctionNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.getPolynomialCoefficients(context, symbolName, coefficients, symbolicComputation);
}

int FunctionNode::getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  /* Since templatedApproximate always evaluates the argument, some apps (such
   * as Statistics and Regression) need to be aware of it even when it does not
   * appear in the formula. */
  nextVariableIndex = childAtIndex(0)->getVariables(context, isVariable, variables, maxSizeVariable, nextVariableIndex);
  if (e.isUninitialized()) {
    return nextVariableIndex;
  }
  return e.node()->getVariables(context, isVariable, variables, maxSizeVariable, nextVariableIndex);
}

Layout FunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Function(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int FunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression FunctionNode::shallowReduce(ReductionContext reductionContext) {
  return Function(this).shallowReduce(reductionContext); // This uses Symbol::shallowReduce
}

Expression FunctionNode::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  return Function(this).deepReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount);
}

Evaluation<float> FunctionNode::approximate(SinglePrecision p, ApproximationContext approximationContext) const {
  return templatedApproximate<float>(approximationContext);
}

Evaluation<double> FunctionNode::approximate(DoublePrecision p, ApproximationContext approximationContext) const {
  return templatedApproximate<double>(approximationContext);
}

template<typename T>
Evaluation<T> FunctionNode::templatedApproximate(ApproximationContext approximationContext) const {
  if (childAtIndex(0)->approximate((T)1, approximationContext).isUndefined()) {
    return Complex<T>::Undefined();
  }

  Function f(this);
  Expression e = SymbolAbstract::Expand(f, approximationContext.context(), true);
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
    if (!p.isUninitialized() && p.node()->childAtIndexNeedsUserParentheses(value, p.indexOfChild(*this))) {
      value = Parenthesis::Builder(value);
    }
    replaceWithInPlace(value);
    return value;
  }
  return *this;
}

Expression Function::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  if (reductionContext.symbolicComputation() == ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined
      || childAtIndex(0).isUndefined())
  {
    return replaceWithUndefinedInPlace();
  }
  if (reductionContext.symbolicComputation() == ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol) {
    return *this;
  }
  Expression result = SymbolAbstract::Expand(*this, reductionContext.context(), true, reductionContext.symbolicComputation());
  if (result.isUninitialized()) {
    if (reductionContext.symbolicComputation() != ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined) {
      return *this;
    }
    return replaceWithUndefinedInPlace();
  }
  replaceWithInPlace(result);
  // The stored expression is as entered by the user, so we need to call reduce
  return result.deepReduce(reductionContext);
}

Expression Function::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  {
    // Replace replaceable symbols in child
    Expression self = defaultReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount);
    if (self.isUninitialized()) { // if the child is circularly defined, escape
      return self;
    }
    assert(*this == self);
  }
  Expression e = context->expressionForSymbolAbstract(*this, false);
  if (e.isUninitialized()) {
    return *this;
  }
  // If the function contains itself, return undefined
  if (e.hasExpression([](Expression e, const void * context) {
          if (e.type() != ExpressionNode::Type::Function) {
            return false;
          }
          return strcmp(static_cast<Function&>(e).name(), reinterpret_cast<const char *>(context)) == 0;
        }, reinterpret_cast<const void *>(name())))
  {
    return Expression();
  }
  replaceWithInPlace(e);
  *didReplace = true;
  return e;
}

}
