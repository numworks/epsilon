#include <poincare/function.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

Expression FunctionNode::replaceSymbolWithExpression(const Symbol & symbol, const Expression & expression) {
  return Function(this).replaceSymbolWithExpression(symbol, expression);
}

int FunctionNode::polynomialDegree(Context & context, const char * symbolName) const {
  Expression e = context.expressionForSymbol(Function(this));
  if (e.isUninitialized()) {
    return -1;
  }
/*TODO Context should be float or double ?*/
  VariableContext newContext = xContext(context);
  return e.polynomialDegree(newContext, symbolName);
}

int FunctionNode::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  Expression e = context.expressionForSymbol(Function(this));
  if (e.isUninitialized()) {
    return -1;
  }
  VariableContext newContext = xContext(context);
  return e.getPolynomialCoefficients(newContext, symbolName, coefficients);
}

int FunctionNode::getVariables(Context & context, isVariableTest isVariable, char * variables, int maxSizeVariable) const {
  Expression e = context.expressionForSymbol(Function(this));
  if (e.isUninitialized()) {
    return 0;
  }
  VariableContext newContext = xContext(context);
  return e.getVariables(newContext, isVariable, variables, maxSizeVariable);
}

float FunctionNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  if (e.isUninitialized()) {
    return 0.0f;
  }
  VariableContext newContext = xContext(context);
  return e.characteristicXRange(newContext, angleUnit);
}

VariableContext FunctionNode::xContext(Context & parentContext) const {
  const char x[] = {Symbol::SpecialSymbols::UnknownX, 0};
  VariableContext xContext = VariableContext(x, &parentContext);
  xContext.setExpressionForSymbol(Expression(childAtIndex(0)), Symbol(x, 1), xContext);
  return xContext;
}

Layout FunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Function(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int FunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression FunctionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Function(this).shallowReduce(context, angleUnit, replaceSymbols); // This uses Symbol::shallowReduce
}

Evaluation<float> FunctionNode::approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  if (e.isUninitialized()) {
    return Complex<float>::Undefined();
  }
  VariableContext newContext = xContext(context);
  return e.approximateToEvaluation<float>(newContext, angleUnit);
}

Evaluation<double> FunctionNode::approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  if (e.isUninitialized()) {
    return Complex<double>::Undefined();
  }
  VariableContext newContext = xContext(context);
  return e.approximateToEvaluation<double>(newContext, angleUnit);
}

Function::Function(const char * name) :
  Function(TreePool::sharedPool()->createTreeNode<FunctionNode>())
{
  static_cast<FunctionNode *>(Expression::node())->setName(name, strlen(name));
}

Expression Function::replaceSymbolWithExpression(const Symbol & symbol, const Expression & expression) {
  // Replace the symbol in the child
  childAtIndex(0).replaceSymbolWithExpression(symbol, expression);
  if (symbol.type() == ExpressionNode::Type::Function && strcmp(name(), symbol.name()) == 0) {
    Expression value = expression.clone();
    // Replace the unknown in the new expression by the function's child
    const char x[2] = {SpecialSymbols::UnknownX, 0};
    Symbol xSymbol = Symbol(x, 1);
    Expression xValue = childAtIndex(0);
    value.replaceSymbolWithExpression(xSymbol, xValue);
    Expression p = parent();
    if (!p.isUninitialized() && p.node()->childNeedsParenthesis(value.node())) {
      value = Parenthesis(value);
    }
    replaceWithInPlace(value);
    return value;
  }
  return *this;
}

}
