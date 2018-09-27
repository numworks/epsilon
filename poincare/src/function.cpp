#include <poincare/function.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/rational.h>
#include <cmath>

namespace Poincare {

int FunctionNode::polynomialDegree(Context & context, const char * symbolName) const {
  Expression e = context.expressionForSymbol(Function(this));
/*TODO Context should be float or double ?*/
  VariableContext newContext = xContext(context);
  return e.polynomialDegree(newContext, symbolName);
}

int FunctionNode::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  Expression e = context.expressionForSymbol(Function(this));
  VariableContext newContext = xContext(context);
  return e.getPolynomialCoefficients(newContext, symbolName, coefficients);
}

int FunctionNode::getVariables(Context & context, isVariableTest isVariable, char * variables[], int maxSizeVariable) const {
  Expression e = context.expressionForSymbol(Function(this));
  VariableContext newContext = xContext(context);
  return e.getVariables(newContext, isVariable, variables, maxSizeVariable);
}

float FunctionNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  VariableContext newContext = xContext(context);
  return e.characteristicXRange(newContext, angleUnit);
}

VariableContext FunctionNode::xContext(Context & parentContext) const {
  const char x[] = {Symbol::SpecialSymbols::UnknownX, 0};
  VariableContext xContext = VariableContext(x, &parentContext);
  xContext.setExpressionForSymbolName(Expression(childAtIndex(0)), x, xContext);
  return xContext;
}

Layout FunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Function(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int FunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression FunctionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Function(this).shallowReduce(context, angleUnit);
}

Evaluation<float> FunctionNode::approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  VariableContext newContext = xContext(context);
  return e.approximateToEvaluation<float>(newContext, angleUnit);
}

Evaluation<double> FunctionNode::approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  VariableContext newContext = xContext(context);
  return e.approximateToEvaluation<double>(newContext, angleUnit);
}

Function::Function(const char * name) :
  Function(TreePool::sharedPool()->createTreeNode<FunctionNode>())
{
  static_cast<FunctionNode *>(Expression::node())->setName(name, strlen(name));
}

Expression Function::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Expression::defaultShallowReduce(context, angleUnit);
}

}
