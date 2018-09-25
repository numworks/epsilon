#include <poincare/function.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/rational.h>
#include <cmath>

namespace Poincare {

int FunctionNode::polynomialDegree(Context & context, char symbolName) const {
  Expression e = context.expressionForSymbol(Function(this));
  return e.polynomialDegree(context, symbolName);
}

int FunctionNode::getPolynomialCoefficients(Context & context, char symbolName, Expression coefficients[]) const {
  Expression e = context.expressionForSymbol(Function(this));
  return e.getPolynomialCoefficients(context, symbolName, coefficients);
}

int FunctionNode::getVariables(Context & context, isVariableTest isVariable, char * variables) const {
  Expression e = context.expressionForSymbol(Function(this));
  return e.getVariables(context, isVariable, variables);
}

float FunctionNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  return e.characteristicXRange(context, angleUnit);
}

Layout FunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char nameString[2] = {name(), 0}; //TODO Fix this when longer name
  return LayoutHelper::Prefix(Function(this), floatDisplayMode, numberOfSignificantDigits, nameString);
}

int FunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char nameString[2] = {name(), 0}; //TODO Fix this when longer name
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, nameString);
}

Expression FunctionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Function(this).shallowReduce(context, angleUnit);
}

Evaluation<float> FunctionNode::approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  return e.approximateToEvaluation<float>(context, angleUnit);
}

Evaluation<double> FunctionNode::approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = context.expressionForSymbol(Function(this));
  return e.approximateToEvaluation<double>(context, angleUnit);
}

Function::Function() : Function(TreePool::sharedPool()->createTreeNode<FunctionNode>()) {}

Expression Function::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Expression::defaultShallowReduce(context, angleUnit);
}

}
