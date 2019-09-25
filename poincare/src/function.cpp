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

int FunctionNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.getPolynomialCoefficients(context, symbolName, coefficients);
}

int FunctionNode::getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return 0;
  }
  return e.getVariables(context, isVariable, variables, maxSizeVariable);
}

float FunctionNode::characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f,context, true);
  if (e.isUninitialized()) {
    return 0.0f;
  }
  return e.characteristicXRange(context, angleUnit);
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

Expression FunctionNode::deepReplaceReplaceableSymbols(Context * context, bool * didReplace) {
  return Function(this).deepReplaceReplaceableSymbols(context, didReplace);
}

Evaluation<float> FunctionNode::approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return templatedApproximate<float>(context, complexFormat, angleUnit);
}

Evaluation<double> FunctionNode::approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return templatedApproximate<double>(context, complexFormat, angleUnit);
}

template<typename T>
Evaluation<T> FunctionNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), context, complexFormat, angleUnit);
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
  if (symbol.type() == ExpressionNode::Type::Function && strcmp(name(), symbol.name()) == 0) {
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
  Function f(*this);
  Expression e = SymbolAbstract::Expand(f, reductionContext.context(), true);
  if (!e.isUninitialized()) {
    replaceWithInPlace(e);
    return e.deepReduce(reductionContext);
  }
  if (!reductionContext.symbolicComputation()) {
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

Expression Function::deepReplaceReplaceableSymbols(Context * context, bool * didReplace) {
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
    return replaceWithUndefinedInPlace();
  }
  replaceWithInPlace(e);
  *didReplace = true;
  return e;
}

}
