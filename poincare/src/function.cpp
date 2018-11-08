#include <poincare/function.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <cmath>

namespace Poincare {

Expression FunctionNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
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
  return templatedApproximate<float>(context, angleUnit);
}

Evaluation<double> FunctionNode::approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const {
  return templatedApproximate<double>(context, angleUnit);
}

template<typename T>
Evaluation<T> FunctionNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  VariableContext newContext = xContext(context);
  return e.approximateToEvaluation<T>(newContext, angleUnit);
}

Function::Function(const char * name, size_t length) :
  Function(TreePool::sharedPool()->createTreeNode<FunctionNode>(SymbolAbstract::AlignedNodeSize(length, sizeof(FunctionNode))))
{
  static_cast<FunctionNode *>(Expression::node())->setName(name, length);
}

Expression Function::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  // Replace the symbol in the child
  childAtIndex(0).replaceSymbolWithExpression(symbol, expression);
  if (symbol.type() == ExpressionNode::Type::Function && strcmp(name(), symbol.name()) == 0) {
    Expression value = expression.clone();
    // Replace the unknown in the new expression by the function's child
    Symbol xSymbol = Symbol(Symbol::SpecialSymbols::UnknownX);
    Expression xValue = childAtIndex(0);
    value = value.replaceSymbolWithExpression(xSymbol, xValue);
    Expression p = parent();
    if (!p.isUninitialized() && p.node()->childNeedsParenthesis(value.node())) {
      value = Parenthesis(value);
    }
    replaceWithInPlace(value);
    return value;
  }
  return *this;
}

Expression Function::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  if (!replaceSymbols) {
    return *this;
  }
  const Expression e = context.expressionForSymbol(*this);
  if (!e.isUninitialized()) {
    // We need to replace the unknown with the child
    Expression result = e.clone();
    Symbol x = Symbol(Symbol::SpecialSymbols::UnknownX);
    result = result.replaceSymbolWithExpression(x, childAtIndex(0));
    replaceWithInPlace(result);
    return result.deepReduce(context, angleUnit, replaceSymbols);
  }
  return *this;
}

}
