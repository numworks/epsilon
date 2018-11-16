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
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.polynomialDegree(context, symbolName);
}

int FunctionNode::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.getPolynomialCoefficients(context, symbolName, coefficients);
}

int FunctionNode::getVariables(Context & context, isVariableTest isVariable, char * variables, int maxSizeVariable) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return 0;
  }
  return e.getVariables(context, isVariable, variables, maxSizeVariable);
}

float FunctionNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
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

Expression FunctionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Function(this).shallowReduce(context, angleUnit, replaceSymbols); // This uses Symbol::shallowReduce
}

Expression FunctionNode::shallowReplaceReplaceableSymbols(Context & context) {
  return Function(this).shallowReplaceReplaceableSymbols(context);
}

Evaluation<float> FunctionNode::approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const {
  return templatedApproximate<float>(context, angleUnit);
}

Evaluation<double> FunctionNode::approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const {
  return templatedApproximate<double>(context, angleUnit);
}

template<typename T>
Evaluation<T> FunctionNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.approximateToEvaluation<T>(context, angleUnit);
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
  Function f(*this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (!e.isUninitialized()) {
    replaceWithInPlace(e);
    return e.deepReduce(context, angleUnit, replaceSymbols);
  }
  return *this;
}

Expression Function::shallowReplaceReplaceableSymbols(Context & context) {
  Expression e = context.expressionForSymbol(*this, true);
  if (e.isUninitialized()) {
    return *this;
  }
  e.replaceSymbolWithExpression(Symbol(Symbol::SpecialSymbols::UnknownX), childAtIndex(0));
  replaceWithInPlace(e);
  return e;
}

// TODO: should we avoid replacing unknown X in-place but use a context instead?
#if 0
VariableContext Function::unknownXContext(Context & parentContext) const {
  Symbol unknownXSymbol(Symbol::SpecialSymbols::UnknownX);
  Expression child = childAtIndex(0);
  const char x[] = {Symbol::SpecialSymbols::UnknownX, 0};
  /* COMMENT */
  if (child.type() == ExpressionNode::Type::Symbol && static_cast<Symbol &>(child).isSystemSymbol()) {
    return parentContext;
  }

  VariableContext xContext = VariableContext(x, &parentContext);

  /* If the parentContext already has an expression for UnknownX, we have to
   * replace in childAtIndex(0) any occurence of UnknownX by its value in
   * parentContext. That way, we handle: evaluatin f(x-1) with x = 2 & f:x->x^2 */
  Expression unknownXValue = parentContext.expressionForSymbol(unknownXSymbol, true);
  if (!unknownXValue.isUninitialized()) {
    xContext = static_cast<VariableContext &>(parentContext); // copy the parentContext
    child.replaceSymbolWithExpression(unknownXSymbol, unknownXValue);
  }
  /* We here assert that child contains no occurrence of UnknownX to avoid
   * creating an infinite loop (for instance: unknownXSymbol = unknownXSymbol+2). */
  assert(!child.recursivelyMatches([](const Expression e, Context & context, bool replaceSymbol) {
        return e.type() == ExpressionNode::Type::Symbol && static_cast<const Symbol &>(e).isSystemSymbol();
      }, parentContext, false));
  xContext.setExpressionForSymbol(child, unknownXSymbol, xContext);
  return xContext;
}
#endif

}
